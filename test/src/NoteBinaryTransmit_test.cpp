/*!
 * @file NoteBinaryTransmit_test.cpp
 *
 * Written by the Blues Inc. team.
 *
 * Copyright (c) 2023 Blues Inc. MIT License. Use of this source code is
 * governed by licenses granted by the copyright holder including that found in
 * the
 * <a href="https://github.com/blues/note-c/blob/master/LICENSE">LICENSE</a>
 * file.
 *
 */

#ifdef NOTE_C_TEST

#include <catch2/catch_test_macros.hpp>
#include "fff.h"

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(J *, NoteNewRequest, const char *)
FAKE_VALUE_FUNC(J *, NoteRequestResponse, J *)
FAKE_VALUE_FUNC(bool, NoteRequest, J *)
FAKE_VALUE_FUNC(const char *, NoteChunkedTransmit, uint8_t *, size_t, bool)
FAKE_VOID_FUNC(NoteLockNote)
FAKE_VOID_FUNC(NoteUnlockNote)

namespace
{

SCENARIO("NoteBinaryTransmit")
{
    RESET_FAKE(NoteNewRequest);
    RESET_FAKE(NoteRequestResponse);
    RESET_FAKE(NoteRequest);
    RESET_FAKE(NoteChunkedTransmit);
    RESET_FAKE(NoteLockNote);
    RESET_FAKE(NoteUnlockNote);

    NoteSetFnDefault(malloc, free, NULL, NULL);

    uint8_t buf[32] = {0xDE, 0xAD, 0xBE, 0xEF};
    size_t dataLen = 4;
    size_t bufLen = sizeof(buf);
    J *req = JCreateObject();
    NoteNewRequest_fake.return_val = req;

    GIVEN("The response to the initial card.binary request to get the length "
          "and max parameters indicates a problem") {
        J *rsp = JCreateObject();
        NoteRequestResponse_fake.return_val = rsp;

        AND_GIVEN("The response is NULL") {
            free(rsp);
            NoteRequestResponse_fake.return_val = NULL;

            WHEN("NoteBinaryTransmit is called") {
                const char *err = NoteBinaryTransmit(buf, dataLen, bufLen,
                    false);

                THEN("An error is returned") {
                    CHECK(err != NULL);
                }
            }
        }

        AND_GIVEN("The response a generic error") {
            JAddStringToObject(rsp, "err", "some error");

            WHEN("NoteBinaryTransmit is called") {
                const char *err = NoteBinaryTransmit(buf, dataLen, bufLen,
                    false);

                THEN("An error is returned") {
                    CHECK(err != NULL);
                }
            }
        }

        AND_GIVEN("The response has an error indicating that the Notecard's "
            "firmware version doesn't support the card.binary API") {
            JAddStringToObject(rsp, "err", "unknown");

            WHEN("NoteBinaryTransmit is called") {
                const char *err = NoteBinaryTransmit(buf, dataLen, bufLen,
                    false);

                THEN("An error is returned") {
                    CHECK(err != NULL);
                }
            }
        }

        AND_GIVEN("The max parameter is 0") {
            JAddIntToObject(rsp, "max", 0);

            WHEN("NoteBinaryTransmit is called") {
                const char *err = NoteBinaryTransmit(buf, dataLen, bufLen,
                    false);

                THEN("An error is returned") {
                    CHECK(err != NULL);
                }
            }
        }

        AND_GIVEN("The remaining space implied by max and length is too small "
            "to hold the caller's data") {

            AND_GIVEN("The length parameter is 0") {
                JAddIntToObject(rsp, "length", 0);
                JAddIntToObject(rsp, "max", dataLen - 1);

                WHEN("NoteBinaryTransmit is called") {
                    const char *err = NoteBinaryTransmit(buf, dataLen, bufLen,
                        false);

                    THEN("An error is returned") {
                        CHECK(err != NULL);
                    }
                }
            }

            AND_GIVEN("The length parameter is non-zero and the buffer is being"
                " appended to existing data") {
                size_t currLen = dataLen;
                JAddIntToObject(rsp, "length", currLen);
                JAddIntToObject(rsp, "max", currLen + dataLen - 1);

                WHEN("NoteBinaryTransmit is called") {
                    const char *err = NoteBinaryTransmit(buf, dataLen, bufLen,
                        true);

                    THEN("An error is returned") {
                        CHECK(err != NULL);
                    }
                }
            }
        }
    }

    GIVEN("The length of the COBS-encoded data is larger than the buffer "
            "size") {
        J *rsp = JCreateObject();
        NoteRequestResponse_fake.return_val = rsp;
        JAddIntToObject(rsp, "length", 0);
        JAddIntToObject(rsp, "max", dataLen);

        AND_GIVEN("The encoded length is exactly the same size as the "
            "buffer, leaving no room for the terminating newline") {
            uint32_t newBufLen = cobsEncodedLength(buf, dataLen);

            WHEN("NoteBinaryTransmit is called") {
                const char *err = NoteBinaryTransmit(buf, dataLen,
                    newBufLen, false);

                THEN("An error is returned") {
                    CHECK(err != NULL);
                }
            }
        }
    }

    GIVEN("The initial card.binary response is ok") {
        J *rsp = JCreateObject();
        NoteRequestResponse_fake.return_val = rsp;
        JAddIntToObject(rsp, "length", 0);
        JAddIntToObject(rsp, "max", 1024);
        uint8_t originalData[sizeof(buf)];
        memcpy(originalData, buf, dataLen);

        AND_GIVEN("Allocating the card.binary.put request fails") {
            J *newReqRetVals[] = {req, NULL};
            SET_RETURN_SEQ(NoteNewRequest, newReqRetVals, 2);

            WHEN("NoteBinaryTransmit is called") {
                const char *err = NoteBinaryTransmit(buf, dataLen, bufLen,
                    false);

                THEN("An error is returned") {
                    CHECK(err != NULL);
                }

                THEN("The input buffer contains the original, unencoded data") {
                    CHECK(memcmp(buf, originalData, dataLen) == 0);
                }
            }
        }

        AND_GIVEN("The card.binary.put request fails") {
            NoteRequest_fake.return_val = false;

            WHEN("NoteBinaryTransmit is called") {
                const char *err = NoteBinaryTransmit(buf, dataLen, bufLen,
                    false);

                THEN("An error is returned") {
                    CHECK(err != NULL);
                }

                THEN("The input buffer contains the original, unencoded data") {
                    CHECK(memcmp(buf, originalData, dataLen) == 0);
                }
            }
        }

        AND_GIVEN("NoteChunkedTransmit fails") {
            NoteRequest_fake.return_val = true;
            NoteChunkedTransmit_fake.return_val = "some error";

            WHEN("NoteBinaryTransmit is called") {
                const char *err = NoteBinaryTransmit(buf, dataLen, bufLen,
                    false);

                THEN("An error is returned") {
                    CHECK(err != NULL);
                }

                THEN("The input buffer contains the original, unencoded data") {
                    CHECK(memcmp(buf, originalData, dataLen) == 0);
                }
            }
        }

        AND_GIVEN("The response to the card.binary request after the "
            "transmission indicates a problem") {
            J *postTransmissionRsp = JCreateObject();
            J *responses[] = {rsp, postTransmissionRsp};
            SET_RETURN_SEQ(NoteRequestResponse, responses, 2);
            NoteRequest_fake.return_val = true;
            NoteChunkedTransmit_fake.return_val = NULL;

            AND_GIVEN("The response is NULL") {
                responses[1] = NULL;

                WHEN("NoteBinaryTransmit is called") {
                    const char *err = NoteBinaryTransmit(buf, dataLen, bufLen,
                        false);

                    THEN("An error is returned") {
                        CHECK(err != NULL);
                    }

                    THEN("The input buffer contains the original, unencoded "
                        "data") {
                        CHECK(memcmp(buf, originalData, dataLen) == 0);
                    }
                }

                free(postTransmissionRsp);
            }

            AND_GIVEN("The response has a generic error") {
                JAddStringToObject(postTransmissionRsp, "err", "some error");

                WHEN("NoteBinaryTransmit is called") {
                    const char *err = NoteBinaryTransmit(buf, dataLen, bufLen,
                        false);

                    THEN("An error is returned") {
                        CHECK(err != NULL);
                    }

                    THEN("The input buffer contains the original, unencoded "
                        "data") {
                        CHECK(memcmp(buf, originalData, dataLen) == 0);
                    }
                }
            }

            AND_GIVEN("The response has a recoverable error") {
                JAddStringToObject(postTransmissionRsp, "err", c_badbinerr);

                WHEN("NoteBinaryTransmit is called") {
                    const char *err = NoteBinaryTransmit(buf, dataLen, bufLen,
                        false);

                    THEN("An error is returned") {
                        CHECK(err != NULL);
                    }

                    THEN("The input buffer contains the original, unencoded "
                        "data") {
                        CHECK(memcmp(buf, originalData, dataLen) == 0);
                    }
                }
            }
        }
    }

    THEN("The Notecard is locked and unlocked the same number of times") {
        CHECK(NoteLockNote_fake.call_count == NoteUnlockNote_fake.call_count);
    }

    JDelete(req);
}

}

#endif // NOTE_C_TEST
