/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 */

#include <glog/logging.h>

#include <fizz/crypto/Utils.h>
#include <folly/init/Init.h>
#include <folly/portability/GFlags.h>

#include <quic/samples/generic/GenericClient.h>
#include <quic/samples/generic/GenericServer.h>


DEFINE_string(host, "::", "Echo server hostname/IP");
DEFINE_int32(p, 6666, "Echo server port");
DEFINE_int32(G, 50000, "Request size for the client");
DEFINE_bool(S, false, "If set, run in server mode");
DEFINE_string(X, "", "sets the keylog file");
DEFINE_string(c, "/certs/cert.crt", "certfile");
DEFINE_string(k, "/certs/key.pem", "private key file");
DEFINE_int32(w, 2*1024*1024, "initial advertised flow control window (for all streams and connection)");
DEFINE_string(CC, "cubic", "congestion control to use (cubic, bbr, copa, newreno, none, ccp)");
DEFINE_string(qlog, "", "path to the qlog file to output");
using namespace quic::samples;

int main(int argc, char* argv[]) {
#if FOLLY_HAVE_LIBGFLAGS
  // Enable glog logging to stderr by default.
  gflags::SetCommandLineOptionWithMode(
      "logtostderr", "1", gflags::SET_FLAGS_DEFAULT);
#endif
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  folly::Init init(&argc, &argv, true);
  fizz::CryptoUtils::init();

  bool server_mode = FLAGS_S;
  const char *keylog_filename = nullptr;

  if (!FLAGS_X.empty()) {
      keylog_filename = FLAGS_X.c_str();
      setenv("SSLKEYLOGFILE", keylog_filename, 1);
  }

  int32_t request_size = FLAGS_G;
  int32_t port = FLAGS_p;


  auto congestionControlOption = quic::congestionControlStrToType(FLAGS_CC);
  if (!congestionControlOption.hasValue()) {
      std::cerr << "wrong congestion control";
      return -2;
  }
  if (server_mode) {
    std::shared_ptr<quic::FileQLogger> qLogger = nullptr;
    if (FLAGS_qlog != "") {
        qLogger = std::make_shared<quic::FileQLogger>(
                quic::VantagePoint::Server, "generic", FLAGS_qlog, true, true);
    }
    GenericServer server("0.0.0.0", port, congestionControlOption.value(), qLogger);
    server.start();
  } else {
    if (argc < 3) {
        std::cerr << "Too few arguments: client expected host and port" << std::endl;
//        print_usage();
        return -2;
    }
    std::string addr(argv[1]);
    int32_t port = strtol(argv[2], nullptr, 10);

    size_t window = FLAGS_w;

    GenericClient client(addr, port, request_size, window);
    client.start(congestionControlOption.value());
  }
  return 0;
}
