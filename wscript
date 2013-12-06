# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

# def configure(conf):
#     conf.check_nonfatal(header_name='stdint.h', define_name='HAVE_STDINT_H')

def build(bld):
    module = bld.create_ns3_module('srp', ['core','internet'])
    module.source = [
        'model/ipv4-srp-routing.cc',
        'model/conf-loader.cc',
        'model/srp-tag.cc',
        'helper/ipv4-srp-routing-helper.cc',
        'util/subnet.cc',
        ]

    module_test = bld.create_ns3_module_test_library('srp')
    module_test.source = [
        'test/srp-test-suite.cc',
        ]

    headers = bld(features='ns3header')
    headers.module = 'srp'
    headers.source = [
        'model/ipv4-srp-routing.h',
        'model/conf-loader.h',
        'model/srp-tag.h',
        'helper/ipv4-srp-routing-helper.h',
        'util/subnet.h',
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    # bld.ns3_python_bindings()

