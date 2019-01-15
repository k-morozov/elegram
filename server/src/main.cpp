#include <boost/program_options/options_description.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <boost/log/core/core.hpp>
#include <boost/log/trivial.hpp>

#include <iostream>

#include "exceptions/exceptions.h"
#include "server/server.h"

namespace {
  void set_up_arguments(const int argc, const char *const *argv,
                        unsigned short &port, bool &logging) {
      namespace po = boost::program_options;

      // Declare the supported options.
      po::options_description desc("Allowed options");
      desc.add_options()
          ("help", "produce help message")
          ("port", po::value<unsigned short>(&port)->required(), "set server port")
          ("logging", po::value<bool>(&logging)->default_value(false), "is logging enabled");

      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, desc), vm);

      if (vm.count("help")) {
          std::cout << desc << "\n";
          throw elegram::exception::terminate_exception("Help information printed");
      }

      po::notify(vm); // after help checking
  }
} // anonymous namespace

int main(const int argc, const char *const argv[]) {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    unsigned short port;
    bool logging_enabled;

    try {
        set_up_arguments(argc, argv, port, logging_enabled);
    } catch (const std::invalid_argument &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const elegram::exception::terminate_exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_SUCCESS;
    }

    boost::log::core::get()->set_logging_enabled(logging_enabled);
    std::shared_ptr<elegram::server::ElegramServer>
        server = std::make_shared<elegram::server::ElegramServer>(port);
    server->run();

    BOOST_LOG_TRIVIAL(info) << "Will return EXIT_SUCCESS now";

    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();

    return EXIT_SUCCESS;
}
