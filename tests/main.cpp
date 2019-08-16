#include "tst_jsonio.h"
#include "tst_service.h"
#include "tst_detail.h"
#include "tst_dump.h"
#include "tst_builder.h"
#include "tst_parser.h"
#include "tst_base.h"


#include <gtest/gtest.h>

// https://doc.qt.io/qtcreator/creator-autotest.html
// https://stackoverflow.com/questions/39574360/google-testing-framework-and-qt
// https://stackoverflow.com/questions/49756620/including-google-tests-in-qt-project


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
