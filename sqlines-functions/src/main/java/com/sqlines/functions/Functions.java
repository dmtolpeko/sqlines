/*
 * Copyright (c) 2022 SQLines
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

package com.sqlines.functions;

/**
 * The class {@code Functions} contains methods for implementing
 * basic SQL built-in functions in Java code rather than in the database.
 */

public class Functions {
    
  /**
   * NVL function - returns first non-null value
   */
  public static <T> T nvl(T expr1, T expr2) {
    return (expr1 != null) ? expr1 : expr2;
  }
}