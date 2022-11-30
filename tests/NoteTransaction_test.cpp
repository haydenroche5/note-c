/*!
 * @file NoteTransaction_test.cpp
 *
 * Written by the Blues Inc. team.
 *
 * Copyright (c) 2022 Blues Inc. MIT License. Use of this source code is
 * governed by licenses granted by the copyright holder including that found in
 * the
 * <a href="https://github.com/blues/note-c/blob/master/LICENSE">LICENSE</a>
 * file.
 *
 */

#include <gtest/gtest.h>
#include "fff.h"

#include "note.h"
#include "n_lib.h"

DEFINE_FFF_GLOBALS;
// These note-c functions are mocked for the purposes of testing
// NoteTransaction.
FAKE_VALUE_FUNC(bool, NoteReset)
FAKE_VALUE_FUNC(const char *, NoteJSONTransaction, char *, char **)

namespace
{

class NoteTransactionTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        RESET_FAKE(NoteReset);
        RESET_FAKE(NoteJSONTransaction);

        // NoteReset's mock should succeed unless the test explicitly instructs
        // it to fail.
        NoteReset_fake.return_val = true;
    }

    void TearDown() override
    {
    }
};

const char *NoteJSONTransactionValid(char *req, char **resp)
{
    static char respString[] = "{ \"total\": 1 }";

    if (resp) {
        char* respBuf = reinterpret_cast<char *>(malloc(sizeof(respString)));
        memcpy(respBuf, respString, sizeof(respString));
        *resp = respBuf;
    }

    return NULL;
}

const char *NoteJSONTransactionError(char *req, char **resp)
{
    return "This is an error.";
}

const char *NoteJSONTransactionBadJSON(char *req, char **resp)
{
    static char respString[] = "Bad JSON";

    if (resp) {
        char* respBuf = reinterpret_cast<char *>(malloc(sizeof(respString)));
        memcpy(respBuf, respString, sizeof(respString));
        *resp = respBuf;
    }

    return NULL;
}

TEST_F(NoteTransactionTest, NullRequest)
{
    EXPECT_EQ(NoteTransaction(NULL), nullptr);
}

TEST_F(NoteTransactionTest, ResponseExpectedAndValid)
{
    J *req = NoteNewRequest("note.add");
    ASSERT_NE(req, nullptr);
    NoteJSONTransaction_fake.custom_fake = NoteJSONTransactionValid;

    J *resp = NoteTransaction(req);

    EXPECT_EQ(NoteJSONTransaction_fake.call_count, 1);
    ASSERT_NE(resp, nullptr);
    // Ensure there's no error in the response.
    EXPECT_FALSE(NoteResponseError(resp));

    JDelete(req);
    JDelete(resp);
}

TEST_F(NoteTransactionTest, ResponseExpectedAndError)
{
    J *req = NoteNewRequest("note.add");
    ASSERT_NE(req, nullptr);
    NoteJSONTransaction_fake.custom_fake = NoteJSONTransactionError;

    J *resp = NoteTransaction(req);

    EXPECT_EQ(NoteJSONTransaction_fake.call_count, 1);
    // Ensure there's an error in the response.
    ASSERT_NE(resp, nullptr);
    EXPECT_TRUE(NoteResponseError(resp));

    JDelete(req);
    JDelete(resp);
}

TEST_F(NoteTransactionTest, ResetRequiredAndResetFails)
{
    J *req = NoteNewRequest("note.add");
    ASSERT_NE(req, nullptr);
    NoteResetRequired();
    // Force NoteReset failure.
    NoteReset_fake.return_val = false;

    J *resp = NoteTransaction(req);

    EXPECT_EQ(NoteReset_fake.call_count, 1);
    // The transaction shouldn't be attempted if reset failed.
    EXPECT_EQ(NoteJSONTransaction_fake.call_count, 0);
    // The response should be null if reset failed.
    EXPECT_EQ(resp, nullptr);

    JDelete(req);
}

TEST_F(NoteTransactionTest, JsonSerializationFails)
{
    // Create an invalid J object.
    J *req = reinterpret_cast<J *>(malloc(sizeof(J)));
    ASSERT_NE(req, nullptr);
    memset(req, 0, sizeof(J));

    J *resp = NoteTransaction(req);

    // The transaction shouldn't be attempted if the request couldn't be
    // serialized.
    EXPECT_EQ(NoteJSONTransaction_fake.call_count, 0);
    // Ensure there's an error in the response.
    ASSERT_NE(resp, nullptr);
    EXPECT_TRUE(NoteResponseError(resp));

    JDelete(req);
    JDelete(resp);
}

TEST_F(NoteTransactionTest, NoResponseExpected)
{
    J *req = NoteNewCommand("note.add");
    ASSERT_NE(req, nullptr);
    NoteJSONTransaction_fake.custom_fake = NoteJSONTransactionValid;

    J *resp = NoteTransaction(req);

    EXPECT_EQ(NoteJSONTransaction_fake.call_count, 1);
    ASSERT_NE(resp, nullptr);
    // Ensure there's no error in the response.
    EXPECT_FALSE(NoteResponseError(resp));
    // Ensure a blank object was returned.
    J *expectedResp = JCreateObject();
    EXPECT_TRUE(JCompare(resp, expectedResp, true));

    JDelete(req);
    JDelete(resp);
    JDelete(expectedResp);
}

TEST_F(NoteTransactionTest, JsonResponseParsingFails)
{
    J *req = NoteNewRequest("note.add");
    ASSERT_NE(req, nullptr);
    NoteJSONTransaction_fake.custom_fake = NoteJSONTransactionBadJSON;

    J *resp = NoteTransaction(req);

    EXPECT_EQ(NoteJSONTransaction_fake.call_count, 1);
    ASSERT_NE(resp, nullptr);
    // Ensure there's an error in the response.
    EXPECT_TRUE(NoteResponseError(resp));

    JDelete(req);
    JDelete(resp);
}

}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    NoteSetFnDefault(malloc, free, NULL, NULL);

    return RUN_ALL_TESTS();
}
