/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include <libinit_kona.h>

using android::base::GetProperty;

void search_variant(const std::vector<variant_info_t> variants) {
    std::string prop;

    for (const auto& variant : variants) {
        prop = GetProperty(variant.prop_key, "");
        if (prop.find(variant.prop_value) != std::string::npos) {
            set_variant_props(variant);
            break;
        }
    }
}

void set_variant_props(const variant_info_t variant) {
    set_ro_build_prop("brand", variant.brand, true);
    set_ro_build_prop("device", variant.device, true);
    set_ro_build_prop("marketname", variant.marketname, true);
    set_ro_build_prop("model", variant.model, true);
    set_ro_build_prop("name", variant.name, true);

    set_ro_build_prop("fingerprint", variant.build_fingerprint);
    property_override("ro.bootimage.build.fingerprint", variant.build_fingerprint.c_str());
    property_override("ro.build.description", variant.build_description.c_str());
}

void property_override(char const prop[], char const value[], bool add) {
    auto pi = (prop_info *) __system_property_find(prop);
    if (pi != nullptr) {
        __system_property_update(pi, value, strlen(value));
    } else if (add) {
        __system_property_add(prop, strlen(prop), value, strlen(value));
    }
}

std::vector<std::string> ro_props_default_source_order = {
    "odm.",
    "product.",
    "system.",
    "system_ext.",
    "vendor.",
    "",
};

void set_ro_build_prop(const std::string &prop, const std::string &value, bool product) {
    std::string prop_name;

    for (const auto &source : ro_props_default_source_order) {
        if (product)
            prop_name = "ro.product." + source + prop;
        else
            prop_name = "ro." + source + "build." + prop;

        property_override(prop_name.c_str(), value.c_str(), true);
    }
}
