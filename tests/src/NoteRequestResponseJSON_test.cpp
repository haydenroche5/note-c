/*!
 * @file NoteRequestResponseJSON_test.cpp
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

#include <catch2/catch_test_macros.hpp>
#include "fff.h"

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(J *, NoteRequestResponse, J *)
FAKE_VALUE_FUNC(char *, JPrintUnformatted, const J *)

namespace
{

J *NoteRequestResponseNULL(J *req)
{
    if (req != NULL) {
        JDelete(req);
    }

    return NULL;
}

J *NoteRequestResponseValid(J *req)
{
    if (req != NULL) {
        JDelete(req);
    }

    J *resp = JCreateObject();
    assert(resp != NULL);
    JAddNumberToObject(resp, "total", 1);

    return resp;
}

char *JPrintUnformattedValid(const J *)
{
    static const char respSrc[] = "{\"total\": 1}";
    char *respDst;

    respDst = (char *)NoteMalloc(sizeof(respSrc));
    assert(respDst != NULL);
    memcpy(respDst, respSrc, sizeof(respSrc));

    return respDst;
}


TEST_CASE("NoteRequestResponseJSON")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);

    RESET_FAKE(NoteRequestResponse);
    RESET_FAKE(JPrintUnformatted);

    SECTION("Passing a NULL request returns NULL")
    {
        REQUIRE(NoteRequestResponseJSON(NULL) == NULL);
    }


    SECTION("Invalid request JSON")
    {
        char req[] = "note.add";
        REQUIRE(NoteRequestResponseJSON(req) == NULL);
    }

    SECTION("NULL response")
    {
        char req[] = "{\"req\": \"note.add\"}";
        NoteRequestResponse_fake.custom_fake = NoteRequestResponseNULL;

        REQUIRE(NoteRequestResponseJSON(req) == NULL);
        REQUIRE(NoteRequestResponse_fake.call_count == 1);
    }

    SECTION("JPrintUnformatted returns NULL")
    {
        char req[] = "{\"req\": \"note.add\"}";
        NoteRequestResponse_fake.custom_fake = NoteRequestResponseValid;
        JPrintUnformatted_fake.return_val = NULL;

        REQUIRE(NoteRequestResponseJSON(req) == NULL);
        REQUIRE(NoteRequestResponse_fake.call_count == 1);
        REQUIRE(JPrintUnformatted_fake.call_count == 1);
    }

    SECTION("Valid response")
    {
        char req[] = "{\"req\": \"note.add\"}";
        NoteRequestResponse_fake.custom_fake = NoteRequestResponseValid;
        JPrintUnformatted_fake.custom_fake = JPrintUnformattedValid;

        char *resp = NoteRequestResponseJSON(req);

        REQUIRE(resp != NULL);
        REQUIRE(NoteRequestResponse_fake.call_count == 1);

        NoteFree(resp);
    }
}

}
