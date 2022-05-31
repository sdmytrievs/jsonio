
#include <gtest/gtest.h>

#include "tst_jsonio.h"
#include "tst_service.h"
#include "tst_detail.h"
#include "tst_dump.h"
#include "tst_builder.h"
#include "tst_parser.h"
#include "tst_base.h"
#include "tst_base_api.h"
#include "tst_base_complex.h"
#include "tst_schema.h"
#include "tst_jsonschema.h"
#include "tst_dbquery.h"
#include "spdlog/spdlog.h"


// https://doc.qt.io/qtcreator/creator-autotest.html
// https://stackoverflow.com/questions/39574360/google-testing-framework-and-qt
// https://stackoverflow.com/questions/49756620/including-google-tests-in-qt-project


int main(int argc, char *argv[])
{
    auto ar_logger = spdlog::get("jsonio17");
    ar_logger->set_level(spdlog::level::off);
    spdlog::set_pattern("[%n] [%^%l%$] %v");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
