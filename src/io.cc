// Copyright (C) 2012--2015 Chi-kwan Chan & Lia Medeiros
// Copyright (C) 2012--2015 Steward Observatory
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
#include <cstdlib>

typedef struct {
  size_t c, n;
  Point *p;
} Log;

static Log *l = NULL;

void Data::snapshot()
{
  if(!l) {
    l = (Log *)malloc(sizeof(Log) * n);
    for(size_t i = 0; i < n; ++i) {
      l[i].c = 0;
      l[i].n = 256;
      l[i].p = (Point *)malloc(sizeof(Point) * 256);
    }
  }

  const State *s = host();
  for(size_t i = 0; i < n; ++i) {
    size_t c = l[i].c;
    if(c && l[i].p[c-1].t == s[i].t)
      continue;

    if(c == l[i].n) {
      l[i].p = (Point *)realloc(l[i].p, sizeof(Point) * (l[i].n += 256));
      if(!l[i].p)
	error("NOT ENOUGH MEMORY!!!\n");
    }

    point(l[i].p+c, s+i);
    ++(l[i].c);
  }
}

void Data::output(const char *sname, const char *oname, const Para &para)
{
  if(sname && *sname) {
    debug("Data::output(...): write snapshot \"%s\"\n", sname);

    size_t sum = 0, max = 0;
    for(size_t i = 0; i < n; ++i) {
      size_t c = l[i].c;
      sum += c;
      if(max < c) max = c;
    }

    FILE *file = fopen(sname, "wb");
    if(file) {
      size_t m = sizeof(Point) / sizeof(real);
      fwrite(&n, sizeof(size_t), 1, file);
      fwrite(&m, sizeof(size_t), 1, file);
      for(size_t i = 0; i < n; ++i) {
	size_t c = l[i].c;
	fwrite(&c, sizeof(size_t), 1, file);
	fwrite(l[i].p, sizeof(Point), c, file);
      }
      fclose(file);
    } else
      error("Data::output(): fail to create file \"%s\"\n", sname);
  }

  if(oname && *oname) {
    debug("Data::output(...): write output \"%s\"\n", oname);

    FILE *file = fopen(oname, "wb");
    if(file) {
      output(host(), &para.buf, file);
      fclose(file);
    } else
      error("Data::output(): fail to create file \"%s\"\n", oname);
  }
}
