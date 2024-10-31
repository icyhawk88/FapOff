// toolchain.cpp
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <vector>
#include "flipper_sdk.h"

using namespace emscripten;

class FlipperBuilder {
public:
    FlipperBuilder() {
        sdk_init();
    }

    std::string buildFAP(const std::string& source, const std::string& appName) {
        FAPBuildConfig config;
        config.name = appName;
        config.source = source;
        
        auto result = compile_fap(config);
        if (!result.success) {
            throw std::runtime_error(result.error);
        }
        
        return result.fap_data;
    }

private:
    void sdk_init() {
        // Initialize SDK components
        initialize_compiler();
        load_sdk_components();
    }
};

EMSCRIPTEN_BINDINGS(flipper_builder) {
    class_<FlipperBuilder>("FlipperBuilder")
        .constructor()
        .function("buildFAP", &FlipperBuilder::buildFAP);
}
