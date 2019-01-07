/** 
 * Copyright (c) 2018 SQLines
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// clang-format off

#ifndef sqlines_release_h
#define sqlines_release_h

#include <string>

static const int32_t SQLINES_VERSION_MAJOR{@SQLINES_VERSION_MAJOR@};
static const int32_t SQLINES_VERSION_MINOR{@SQLINES_VERSION_MINOR@};
static const int32_t SQLINES_VERSION_PATCH{@SQLINES_VERSION_PATCH@};
static const int32_t SQLINES_VERSION{
  @SQLINES_VERSION_MAJOR@ * 1000000 + @SQLINES_VERSION_MINOR@ * 1000 + @SQLINES_VERSION_PATCH@
};

static const std::string SQLINES_VERSION_EXTRA{"@SQLINES_VERSION_EXTRA@"};
static const std::string SQLINES_VERSION_RAW{"@SQLINES_VERSION_RAW@"};
static const std::string SQLINES_BUILD_DATE{"@SQLINES_BUILD_DATE@"};
static const std::string SQLINES_GIT_HASH{"@SQLINES_GIT_HASH@"};

static const std::string SQLINES_RELEASE{"@SQLINES_VERSION_RAW@-@SQLINES_BUILD_DATE@-@SQLINES_GIT_HASH@"};

#endif // sqlines_release_h
