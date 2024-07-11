// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

#include "libxml/parser.h"
#include "libxml/xmlsave.h"

void ignore (void* ctx, const char* msg, ...) {
  // Error handler to avoid spam of error messages from libxml parser.
}

uint32_t hash_string(const char *str, size_t length) {
    uint32_t hash = 5381;
    for (size_t i = 0; i < length; ++i) {
        hash = ((hash << 5) + hash) + (unsigned char)str[i]; // hash * 33 + c
    }
    return hash;
}

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  xmlSetGenericErrorFunc(NULL, &ignore);

  // Test default empty options value and some random combination.
  char *data_string = malloc(size + 1);
  if(data_string == NULL){
    return 1;
  }
  memcpy(data_string, data, size);
  data_string[size] = '\0';

  uint32_t data_hash = hash_string(data_string, size);
  const int max_option_value = INT_MAX;
  int random_option_value = data_hash % max_option_value;

  // Disable XML_PARSE_HUGE to avoid stack overflow.
  random_option_value &= ~XML_PARSE_HUGE;
  const int options[] = {0, random_option_value};
  //
  for(size_t i = 0; i < sizeof(options) / sizeof(options[0]); i++){
    int option_value = options[i];

    xmlDocPtr doc = xmlReadMemory((const char *)data, size, "noname.xml", NULL, option_value);
    if(doc){
      auto buf = xmlBufferCreate();
      assert(buf);
      xmlSaveCtxtPtr ctxt = xmlSaveToBuffer(buf, NULL, 0);
      xmlSaveDoc(ctxt, doc);
      xmlSaveClose(ctxt);
      xmlFreeDoc(doc);
      xmlBufferFree(buf);
    }
  }
  free(data_string);
  return 0;
}
