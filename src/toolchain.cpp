// src/toolchain.cpp

// Update include path to be relative to source
#include "../../sdk/flipper-sdk/firmware/targets/f7/inc/flipper_sdk.h"


// toolchain.cpp
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <vector>
//#include "sdk/flipperzero-firmware/firmware/targets/f7/inc/flipper_sdk.h"

using namespace emscripten;

class FlipperBuilder {
public:
    FlipperBuilder() {
        init_sdk();
    }

    std::string buildFAP(const std::string& source, const std::string& appName) {
        try {
            return compile_fap(source, appName);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Build failed: ") + e.what());
        }
    }

private:
    void init_sdk() {
        // Initialize SDK components
        LL_Init();
        furi_init();
    }

    std::string compile_fap(const std::string& source, const std::string& appName) {
        // Setup build environment
        std::string build_dir = "/tmp/build_" + appName;
        mkdir(build_dir.c_str(), 0755);

        // Write source file
        std::string source_path = build_dir + "/" + appName + ".c";
        write_file(source_path, source);

        // Write application.fam
        std::string fam = generate_fam(appName);
        write_file(build_dir + "/application.fam", fam);

        // Compile
        int result = system(("emcc " + source_path + " -o " + build_dir + "/app.fap " +
            "-I" + SDK_PATH + "/firmware/targets/f7/inc " +
            "-I" + SDK_PATH + "/firmware/targets/furi_hal_include").c_str());

        if (result != 0) {
            throw std::runtime_error("Compilation failed");
        }

        // Read compiled FAP
        return read_file(build_dir + "/app.fap");
    }

    std::string generate_fam(const std::string& appName) {
        return "App(\n"
               "    appid=\"" + appName + "\",\n"
               "    name=\"" + appName + "\",\n"
               "    apptype=FlipperAppType.EXTERNAL,\n"
               "    entry_point=\"app_main\",\n"
               "    stack_size=2 * 1024,\n"
               "    fap_category=\"Examples\",\n"
               "    fap_version=\"1.0\"\n"
               ")";
    }
};

EMSCRIPTEN_BINDINGS(flipper_builder) {
    class_<FlipperBuilder>("FlipperBuilder")
        .constructor()
        .function("buildFAP", &FlipperBuilder::buildFAP);
}