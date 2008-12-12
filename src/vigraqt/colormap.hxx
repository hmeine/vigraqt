/************************************************************************/
/*                                                                      */
/*                  Copyright 2005-2008 by Hans Meine                   */
/*                   meine@informatik.uni-hamburg.de                    */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*  This file is part of the VigraQt library.                           */
/*                                                                      */
/*  VigraQt is free software; you can redistribute it and/or modify it  */
/*  under the terms of the GNU General Public License as published by   */
/*  the Free Software Foundation; either version 2 of the License, or   */
/*  (at your option) any later version.                                 */
/*                                                                      */
/*  VigraQt is distributed in the hope that it will be useful, but      */
/*  WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                */
/*  See the GNU General Public License for more details.                */
/*                                                                      */
/*  You should have received a copy of the GNU General Public License   */
/*  along with VigraQt; if not, write to the                            */
/*                   Free Software Foundation, Inc.,                    */
/*       51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA         */
/*                                                                      */
/************************************************************************/

#ifndef COLORMAP_HXX
#define COLORMAP_HXX

#include "vigraqt_export.hxx"
#include <vigra/rgbvalue.hxx>

class VIGRAQT_EXPORT ColorMap
{
  public:
    typedef vigra::RGBValue<unsigned char> Color;
    typedef float ArgumentType;

    virtual ~ColorMap();

    void setDomain(ArgumentType min, ArgumentType max);

    virtual ArgumentType domainMin() const = 0;

    virtual ArgumentType domainMax() const = 0;

    virtual Color operator()(ArgumentType v) const = 0;
    
        /**
         * Accessor API - makes it possible to write scalar data into
         * target color image using the color map as accessor.
         */
    template<class ITERATOR>
    inline void set(ArgumentType v, ITERATOR it) const;
};

template<class ITERATOR>
inline void ColorMap::set(ArgumentType v, ITERATOR it) const
{
    *it = operator()(v);
}

VIGRAQT_EXPORT ColorMap *createCM();

#endif // COLORMAP_HXX
