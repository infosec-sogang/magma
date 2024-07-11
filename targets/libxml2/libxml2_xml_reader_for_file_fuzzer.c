// Copyright 2018 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// #include "FuzzedDataProvider.h" ====
typedef struct 
{
  const uint8_t *data_ptr;
  size_t remaining_bytes;
} FuzzedDataProvider;

FuzzedDataProvider *provider(const uint8_t *data, size_t size)
{
  FuzzedDataProvider *ret = (FuzzedDataProvider *)malloc(sizeof(FuzzedDataProvider));
  ret->data_ptr = data;
  ret->remaining_bytes = size;
  return ret;
}

int consumeIntegral(FuzzedDataProvider *p)
{
  int min = INT_MIN;
  int max = INT_MAX;

  uint64_t range = (uint64_t)max - (uint64_t)min;
  uint64_t result = 0;
  size_t offset = 0;

  while (offset < sizeof(int) * CHAR_BIT && (range >> offset) > 0 &&
         p->remaining_bytes != 0)
  {
    // Pull bytes off the end of the seed data.
    --(p->remaining_bytes);
    result = (result << CHAR_BIT) | p->data_ptr[p->remaining_bytes];
    offset += CHAR_BIT;
  }

  // Avoid division by 0, in case |range + 1| results in overflow.
  if (range != UINT64_MAX)
    result = result % (range + 1);

  return (int)(min + result);
}

char convertUnsignedToSigned(uint8_t value) {
    return (char)value;
}

void advance(FuzzedDataProvider *p, size_t count) {
    p->data_ptr += count;
    p->remaining_bytes -= count;
}

char* consumeRandomLengthString(FuzzedDataProvider *p, size_t max_length) {
    // 결과를 저장할 동적 문자열을 할당합니다.
    size_t length = (max_length < (p->remaining_bytes)) ? max_length : (p->remaining_bytes);
    char *result = (char *)malloc(length + 1); // 널 종단을 위해 +1
    if (result == NULL) {
        return NULL;
    }
    size_t result_length = 0;
    for (size_t i = 0; i < max_length && (p->remaining_bytes) != 0; i++) {
        char next = convertUnsignedToSigned((p->data_ptr)[0]);
        advance(p, 1);
        if (next == '\\' && (p->remaining_bytes) != 0) {
            next = convertUnsignedToSigned((p->data_ptr)[0]);
            advance(p, 1);
            if (next != '\\')
                break;
        }
        result[result_length++] = next;
    }

    result[result_length] = '\0'; // 널 종단

    // 필요없는 메모리를 해제합니다.
    result = (char *)realloc(result, result_length + 1);
    if (result == NULL) {
        return NULL;
    }
    return result;
}

typedef struct {
  uint8_t* data;
  size_t size;
} FileContents;

FileContents* consumeRemainingBytes(FuzzedDataProvider *p) {
    size_t size = p->remaining_bytes;
    size_t num_bytes_to_consume = p->remaining_bytes;
    if (sizeof(uint8_t) != sizeof(uint8_t)) {
        // fprintf(stderr, "Incompatible data type.\n");
        return NULL;
    }
    FileContents* ret = (FileContents *)malloc(sizeof(FileContents));
    ret->size = p->remaining_bytes;

    ret->data = (uint8_t *)malloc(size * sizeof(uint8_t));
    if (ret->data == NULL) {
        // fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    if (size == 0) {
        if (num_bytes_to_consume != 0)
            abort();
        return ret;
    }

    memcpy(ret->data, p->data_ptr, num_bytes_to_consume);
    advance(p, num_bytes_to_consume);

    // 실제로는 C에서는 shrink_to_fit과 같은 동작을 할 필요가 없지만, 메모리 최적화를 위해 재할당
    ret->data = (uint8_t *)realloc(ret->data, size * sizeof(uint8_t));
    if (ret->data == NULL) {
        // fprintf(stderr, "Memory reallocation failed\n");
        return NULL;
    }

    return ret;
}


// #include "fuzzer_temp_file.h" ====
typedef struct {
  char *filename;
} FuzzerTemporaryFile;


char* fuzzer_get_tmpfile(const uint8_t* data, size_t size) {
  char* filename_buffer = strdup("/tmp/generate_temporary_file.XXXXXX");
  if (!filename_buffer) {
    perror("Failed to allocate file name buffer.");
    abort();
  }
  const int file_descriptor = mkstemp(filename_buffer);
  if (file_descriptor < 0) {
    perror("Failed to make temporary file.");
    abort();
  }
  FILE* file = fdopen(file_descriptor, "wb");
  if (!file) {
    perror("Failed to open file descriptor.");
    close(file_descriptor);
    abort();
  }
  const size_t bytes_written = fwrite(data, sizeof(uint8_t), size, file);
  if (bytes_written < size) {
    close(file_descriptor);
    // fprintf(stderr, "Failed to write all bytes to file (%zu out of %zu)",
    //         bytes_written, size);
    abort();
  }
  fclose(file);
  return filename_buffer;
}

void fuzzer_release_tmpfile(char* filename) {
  if (unlink(filename) != 0) {
    perror("WARNING: Failed to delete temporary file.");
  }
  free(filename);
}

FuzzerTemporaryFile getFile(const uint8_t* data, size_t size){
  FuzzerTemporaryFile ret;
  ret.filename = fuzzer_get_tmpfile(data, size);
  return ret;
}

// ==================================
#include "libxml/xmlreader.h"

void ignore(void *ctx, const char *msg, ...)
{
  // Error handler to avoid spam of error messages from libxml parser.
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
  xmlSetGenericErrorFunc(NULL, &ignore);

  FuzzedDataProvider *p = provider(data, size);
  const int options = consumeIntegral(p);

  // libxml does not expect more than 100 characters, let's go beyond that.
  const char* encoding = consumeRandomLengthString(p, 128);
  FileContents* file_contents = consumeRemainingBytes(p);

  FuzzerTemporaryFile file = getFile(file_contents->data, file_contents->size);

  xmlTextReaderPtr xmlReader =
      xmlReaderForFile(file.filename, encoding, options);

  int kReadSuccessful = 1;
  while (xmlTextReaderRead(xmlReader) == kReadSuccessful)
  {
    xmlTextReaderNodeType(xmlReader);
    xmlTextReaderConstValue(xmlReader);
  }

  xmlFreeTextReader(xmlReader);
  
  free(p);
  return EXIT_SUCCESS;
}
