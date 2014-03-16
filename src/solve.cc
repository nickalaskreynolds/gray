// Copyright (C) 2012--2014 Chi-kwan Chan
// Copyright (C) 2012--2014 Steward Observatory
//
// This file is part of GRay.
//
// GRay is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GRay is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GRay.  If not, see <http://www.gnu.org/licenses/>.

#include "gray.h"
#include <para.h>

#ifndef DT_DUMP
#define DT_DUMP 1
#endif

#ifdef ENABLE_GL
extern void display(size_t, GLuint);

int solve(Data &data)
{
  debug("solve(*%p)\n", &data);

  while(!glfwWindowShouldClose(global::window)) {
    static size_t count = 0;
    static size_t delta = 32;
    const  size_t limit = 1024;

    if(global::dt_dump != 0.0) {
      double ms;
      if(count + delta < limit) {
        ms = evolve(data, global::dt_dump * delta / limit);
        if(0 == ms) break;
        count += delta;
      } else {
        ms = evolve(data, global::dt_dump * (limit - count) / limit);
        if(0 == ms) break;
        count = 0;
      }
      if(ms < 20 && delta < limit) delta *= 2;
      if(ms > 80 && delta > 1    ) delta /= 2;
    }

#if defined(ENABLE_PRIME) || defined(ENABLE_LEAP)
    sense();
#endif

    display((size_t)data, (GLuint)data);

    glfwSwapBuffers(global::window);
    glfwPollEvents();
  }

  data.spec(global::format); // TODO: check if glutMainLoop() actually exit...
  return 0;
}
#else
int solve(Data &data)
{
  debug("solve(*%p)\n", &data);

  if(global::dt_dump != 0.0) {
    data.dump(global::format, global::t);
    while(0 < evolve(data, global::dt_dump))
      data.dump(global::format, global::t);
  } else
    while(0 < evolve(data, DT_DUMP));

  data.spec(global::format);
  return 0;
}
#endif
