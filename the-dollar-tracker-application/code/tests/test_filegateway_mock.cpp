#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QString>
#include <optional>

using ::testing::Return;

// Mock interface
class MockFileGateway {
public:
    MOCK_METHOD(std::optional<QString>, readAll, (const QString&), ());
    MOCK_METHOD(bool, writeAll, (const QString&, const QString&), ());
};

// Example test using Google Mock
TEST(FileGatewayMockTest, WriteAllIsCalledWithCorrectArguments) {
    MockFileGateway mock;

    QString path = "test.txt";
    QString content = "Hello Payroll";

    EXPECT_CALL(mock, writeAll(path, content))
        .Times(1)
        .WillOnce(Return(true));

    bool result = mock.writeAll(path, content);
    EXPECT_TRUE(result);
}
