/*!
 * @file NoteRequest_test.cpp
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
// NoteRequest.
FAKE_VALUE_FUNC(J *, NoteTransaction, J *)

namespace
{

class NoteRequestTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        RESET_FAKE(NoteTransaction);
    }

    void TearDown() override
    {
    }
};

J *NoteTransactionValid(J *req)
{
    return JCreateObject();
}

J *NoteTransactionError(J *req)
{
    J *resp = JCreateObject();
    assert(resp != NULL);
    JAddStringToObject(resp, c_err, "An error.");

    return resp;
}

TEST_F(NoteRequestTest, NullRequest)
{
    EXPECT_FALSE(NoteRequest(NULL));
}

TEST_F(NoteRequestTest, NoteTransactionNull)
{
    J *req = NoteNewRequest("note.add");
    ASSERT_NE(req, nullptr);
    NoteTransaction_fake.return_val = NULL;

    EXPECT_FALSE(NoteRequest(req));
    EXPECT_EQ(NoteTransaction_fake.call_count, 1);
}

TEST_F(NoteRequestTest, NoteTransactionError)
{
    J *req = NoteNewRequest("note.add");
    ASSERT_NE(req, nullptr);
    NoteTransaction_fake.custom_fake = NoteTransactionError;

    EXPECT_FALSE(NoteRequest(req));
    EXPECT_EQ(NoteTransaction_fake.call_count, 1);
}

TEST_F(NoteRequestTest, NoteTransactionValid)
{
    J *req = NoteNewRequest("note.add");
    ASSERT_NE(req, nullptr);
    NoteTransaction_fake.custom_fake = NoteTransactionValid;

    EXPECT_TRUE(NoteRequest(req));
    EXPECT_EQ(NoteTransaction_fake.call_count, 1);
}

}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    NoteSetFnDefault(malloc, free, NULL, NULL);

    return RUN_ALL_TESTS();
}
