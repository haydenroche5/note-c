// /*!
//  * @file NoteBinaryReceive_test.cpp
//  *
//  * Written by the Blues Inc. team.
//  *
//  * Copyright (c) 2023 Blues Inc. MIT License. Use of this source code is
//  * governed by licenses granted by the copyright holder including that found in
//  * the
//  * <a href="https://github.com/blues/note-c/blob/master/LICENSE">LICENSE</a>
//  * file.
//  *
//  */

// #ifdef NOTE_C_TEST

// #include <catch2/catch_test_macros.hpp>
// #include "fff.h"

// #include "n_lib.h"

// DEFINE_FFF_GLOBALS
// FAKE_VALUE_FUNC(J *, NoteNewRequest, const char *)
// FAKE_VALUE_FUNC(J *, NoteRequestResponse, J *)
// FAKE_VALUE_FUNC(const char *, NoteChunkedReceive, uint8_t *, size_t *, bool,
//     size_t, uint32_t *)
// FAKE_VOID_FUNC(NoteLockNote)
// FAKE_VOID_FUNC(NoteUnlockNote)

// namespace
// {

// SCENARIO("NoteBinaryReceive")
// {
//     RESET_FAKE(NoteNewRequest);
//     RESET_FAKE(NoteRequestResponse);
//     RESET_FAKE(NoteChunkedReceive);
//     RESET_FAKE(NoteLockNote);
//     RESET_FAKE(NoteUnlockNote);

//     NoteSetFnDefault(malloc, free, NULL, NULL);

//     uint8_t buf[32];
//     size_t size = sizeof(buf);
//     J *req = JCreateObject();
//     NoteNewRequest_fake.return_val = req;

//     GIVEN("The initial card.binary request fails") {
//         NoteRequestResponse_fake.return_val = NULL;

//         WHEN("NoteBinaryReceive is called") {
//             const char *err = NoteBinaryReceive(buf, size);

//             THEN("An error is returned") {
//                 CHECK(err != NULL);
//             }
//         }
//     }

//     GIVEN("The response to the initial card.binary request has an error") {
//         J *rsp = JCreateObject();
//         JAddStringToObject(rsp, "err", "some error");
//         NoteRequestResponse_fake.return_val = rsp;

//         WHEN("NoteBinaryReceive is called") {
//             const char *err = NoteBinaryReceive(buf, size);

//             THEN("An error is returned") {
//                 CHECK(err != NULL);
//             }
//         }
//     }

//     GIVEN("The response to the initial card.binary request indicates there's no"
//           "binary data to read") {
//         J *rsp = JCreateObject();
//         JAddIntToObject(rsp, "cobs", 0);
//         NoteRequestResponse_fake.return_val = rsp;

//         WHEN("NoteBinaryReceive is called") {
//             const char *err = NoteBinaryReceive(buf, size);

//             THEN("An error is returned") {
//                 CHECK(err != NULL);
//             }
//         }
//     }

//     GIVEN("The response to the initial card.binary request indicates there's"
//           "more data to read than will fit in the provided buffer") {
//         J *rsp = JCreateObject();
//         JAddIntToObject(rsp, "cobs", size + 1);
//         NoteRequestResponse_fake.return_val = rsp;

//         WHEN("NoteBinaryReceive is called") {
//             const char *err = NoteBinaryReceive(buf, size);

//             THEN("An error is returned") {
//                 CHECK(err != NULL);
//             }
//         }
//     }

//     GIVEN("Allocating the card.binary.get request fails") {
//         J *rsp = JCreateObject();
//         JAddIntToObject(rsp, "cobs", size);
//         J *newReqRetVals[] = {req, NULL};
//         SET_RETURN_SEQ(NoteNewRequest, newReqRetVals, 2);
//         NoteRequestResponse_fake.return_val = rsp;

//         WHEN("NoteBinaryReceive is called") {
//             const char *err = NoteBinaryReceive(buf, size);

//             THEN("An error is returned") {
//                 CHECK(err != NULL);
//             }
//         }
//     }

//     GIVEN("Response to card.binary.get contains an error") {
//         J *cardBinaryRsp = JCreateObject();
//         JAddIntToObject(cardBinaryRsp, "cobs", size);
//         J *cardBinaryGetRsp = JCreateObject();
//         JAddStringToObject(cardBinaryGetRsp, "err", "some error");
//         J *responses[] = {cardBinaryRsp, cardBinaryGetRsp};
//         SET_RETURN_SEQ(NoteRequestResponse, responses, 2);
//         NoteNewRequest_fake.return_val = req;

//         WHEN("NoteBinaryReceive is called") {
//             const char *err = NoteBinaryReceive(buf, size);

//             THEN("An error is returned") {
//                 CHECK(err != NULL);
//             }
//         }
//     }

//     GIVEN("NoteChunkedReceive returns an error") {
//         J *cardBinaryRsp = JCreateObject();
//         JAddIntToObject(cardBinaryRsp, "cobs", size);
//         J *cardBinaryGetRsp = JCreateObject();
//         JAddStringToObject(cardBinaryGetRsp, "status", "");
//         J *responses[] = {cardBinaryRsp, cardBinaryGetRsp};
//         SET_RETURN_SEQ(NoteRequestResponse, responses, 2);

//         NoteNewRequest_fake.return_val = req;
//         NoteChunkedReceive_fake.return_val = "some error";

//         WHEN("NoteBinaryReceive is called") {
//             const char *err = NoteBinaryReceive(buf, size);

//             THEN("An error is returned") {
//                 CHECK(err != NULL);
//             }
//         }
//     }

//     GIVEN("NoteChunkedReceive indicates there's unexpectedly more data "
//         "available") {
//         J *cardBinaryRsp = JCreateObject();
//         JAddIntToObject(cardBinaryRsp, "cobs", size);
//         J *cardBinaryGetRsp = JCreateObject();
//         JAddStringToObject(cardBinaryGetRsp, "status", "");
//         J *responses[] = {cardBinaryRsp, cardBinaryGetRsp};
//         SET_RETURN_SEQ(NoteRequestResponse, responses, 2);

//         NoteNewRequest_fake.return_val = req;
//         NoteChunkedReceive_fake.custom_fake = [](uint8_t *, size_t *, bool,
//             size_t, uint32_t *available) -> const char*
//         {
//             *available = 1;

//             return NULL;
//         };

//         WHEN("NoteBinaryReceive is called") {
//             const char *err = NoteBinaryReceive(buf, size);

//             THEN("An error is returned") {
//                 CHECK(err != NULL);
//             }
//         }
//     }

//     GIVEN("The binary payload is received") {
//         J *cardBinaryRsp = JCreateObject();
//         JAddIntToObject(cardBinaryRsp, "cobs", size);
//         J *cardBinaryGetRsp = JCreateObject();
//         J *responses[] = {cardBinaryRsp, cardBinaryGetRsp};
//         SET_RETURN_SEQ(NoteRequestResponse, responses, 2);

//         NoteNewRequest_fake.return_val = req;
//         NoteChunkedReceive_fake.custom_fake = [](uint8_t *buffer, size_t *size,
//             bool, size_t, uint32_t *available) -> const char*
//         {
//             char rawMsg[] = "Hello Blues!";
//             size_t rawMsgLen = strlen(rawMsg);
//             uint32_t encodedMaxLen = cobsEncodedMaxLength(rawMsgLen);
//             uint8_t *encodeBuf = (uint8_t *)malloc(encodedMaxLen);
//             uint32_t encodedLen = cobsEncode((uint8_t *)rawMsg, rawMsgLen, '\n',
//                 encodeBuf);

//             memcpy(buffer, encodeBuf, encodedLen);
//             buffer[encodedLen] = '\n';
//             *size = encodedLen + 1;
//             *available = 0;

//             return NULL;
//         };

//         AND_GIVEN("The computed MD5 hash doesn't match the status field") {
//             JAddStringToObject(cardBinaryGetRsp, "status", "garbage");

//             WHEN("NoteBinaryReceive is called") {
//                 const char *err = NoteBinaryReceive(buf, size);

//                 THEN("An error is returned") {
//                     CHECK(err != NULL);
//                 }
//             }
//         }

//         AND_GIVEN("The computed MD5 matches the status field") {
//             char hash[NOTE_MD5_HASH_STRING_SIZE] = {0};
//             NoteMD5HashString(rawMsg, rawMsgLen, hash,
//                 NOTE_MD5_HASH_STRING_SIZE);
//             JAddStringToObject(cardBinaryGetRsp, "status", hash);

//             WHEN("NoteBinaryReceive is called") {
//                 const char *err = NoteBinaryReceive(buf, size);

//                 THEN("No error is returned") {
//                     CHECK(err == NULL);
//                 }
//             }
//         }
//     }

//     THEN("The Notecard is locked and unlocked the same number of times") {
//         CHECK(NoteLockNote_fake.call_count == NoteUnlockNote_fake.call_count);
//     }

//     JDelete(req);
// }

// }

// #endif // NOTE_C_TEST
