/************************************************************************/
/*                                                                      */
/*                  Copyright 2005-2006 by Hans Meine                   */
/*                 meine@kogs.informatik.uni-hamburg.de                 */
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
#include <vigra/numerictraits.hxx>
#include <vigra/rgbvalue.hxx>
#include <vector>

// TODO: template??
//template<class COLOR = vigra::RGBValue<unsigned char> >
class VIGRAQT_EXPORT ColorMap
{
  public:
    typedef vigra::RGBValue<unsigned char> Color;
    typedef float ArgumentType;

    void setDomain(ArgumentType min, ArgumentType max);

    inline ArgumentType domainMin() const
    { return transitionPoints_.front().projected; }

    inline ArgumentType domainMax() const
    { return transitionPoints_.back().projected; }

    inline Color operator()(ArgumentType v) const;

    template<class ITERATOR>
    inline void set(ArgumentType v, ITERATOR it) const;

    unsigned int size() const
    {
        return transitionPoints_.size();
    }

    double position(unsigned int i) const
    {
        return transitionPoints_[i].position;
    }

    double domainPosition(unsigned int i) const
    {
        return transitionPoints_[i].projected;
    }
    void setDomainPosition(unsigned int i, double v);

    Color color(unsigned int i) const
    {
        return vigra::NumericTraits<Color>::fromRealPromote(
            transitionPoints_[i].color);
    }
    void setColor(unsigned int i, Color c);

        // TODO: improved name indicating domain position?
    unsigned int insert(double domainPosition);

    void remove(unsigned int i);

        /**
         * TransitionIterator is an iterator over groups of transition
         * points within a ColorMap.  This is similar to an iterator
         * over each transition point, but it groups together
         * transition points with the same position (which define
         * "sharp" step transitions.
         */
    class TransitionIterator
    {
      public:
        unsigned int firstIndex() const
        {
            return i1_;
        }

        unsigned int lastIndex() const
        {
            return i2_;
        }

        bool inRange() const
        {
            return i1_ < colorMap_->size();
        }

        bool atEnd() const
        {
            return !inRange();
        }

            /**
             * Returns true iff there is a sharp transition at this
             * position.  This is the case if there are multiple
             * colors defined for the same position, i.e. lastIndex()
             * != firstIndex().
             */
        bool isStepTransition() const
        {
            return lastIndex() > firstIndex();
        }

        double position() const
        {
            return colorMap_->position(firstIndex());
        }

        double domainPosition() const
        {
            return colorMap_->domainPosition(firstIndex());
        }

            /**
             * Return the color of this transition point on the left
             * side.  If there are multiple colors defined for the
             * same position, this returns the first one (i.e. that of
             * firstIndex()).
             */
        Color leftColor() const
        {
            return colorMap_->color(firstIndex());
        }

            /**
             * Return the color of this transition point on the right
             * side.  If there are multiple colors defined for the
             * same position, this returns the last one (i.e. that of
             * lastIndex()).
             */
        Color rightColor() const
        {
            return colorMap_->color(lastIndex());
        }

            /**
             * Returns true iff the range before this transition point
             * has a constant color.  This is the case if this is
             * either the first transition point or the previous
             * transition point has the same rightColor() as this
             * leftColor().
             */
        bool leftFlat() const
        {
            return firstIndex() <= 0 || (
                colorMap_->color(firstIndex()-1) == leftColor());
        }

            /**
             * Returns true iff the range after this transition point
             * has a constant color.  This is the case if this is
             * either the last transition point or the next transition
             * point has the same leftColor() as this rightColor().
             */
        bool rightFlat() const
        {
            return lastIndex() >= colorMap_->size()-1 || (
                colorMap_->color(lastIndex()+1) == rightColor());
        }

        TransitionIterator &operator++()
        {
            vigra_assert(!atEnd(), "must not advance if atEnd()!");
            i1_ = i2_ + 1;
            advanceRight();
            return *this;
        }

        TransitionIterator operator++(int)
        {
            TransitionIterator ret(*this);
            operator++();
            return ret;
        }

        bool operator==(TransitionIterator const &other) const
        {
            return i1_ == other.i1_;
        }

        bool operator!=(TransitionIterator const &other) const
        {
            return i1_ != other.i1_;
        }

      protected:
        friend class ColorMap;

        TransitionIterator(const ColorMap *colorMap, unsigned int index)
        : colorMap_(colorMap),
          i1_(index)
        {
            advanceRight();
        }

        void advanceRight()
        {
            i2_ = i1_;
            while(i2_ < colorMap_->size()-1 &&
                  colorMap_->position(i2_+1) == colorMap_->position(i1_))
                ++i2_;
        }

        const ColorMap *colorMap_;
        unsigned int i1_, i2_;
    };

    TransitionIterator transitionsBegin() const
    {
        return TransitionIterator(this, 0);
    }

    TransitionIterator transitionsEnd() const
    {
        return TransitionIterator(this, size());
    }

  protected:
    void recalculateFactors();

    typedef vigra::NumericTraits<Color>::RealPromote InternalColor;

    struct TransitionPoint
    {
        double position, projected;
        InternalColor color, scale;

        TransitionPoint(double position, Color color)
        : position(position), projected(0.0), color(color)
        {}
    };

    typedef std::vector<TransitionPoint> TransitionPoints;

    TransitionPoints transitionPoints_;
};

ColorMap *createCM();

//template<class COLOR = vigra::RGBValue<unsigned char> >
inline ColorMap::Color ColorMap::operator()(ArgumentType v) const
{
    TransitionPoints::const_iterator
        tpIt(transitionPoints_.begin());

    if(v < tpIt->projected)
    {
        return vigra::NumericTraits<Color>::fromRealPromote(tpIt->color);
    }

    TransitionPoints::const_iterator
        prevTP(tpIt), tpEnd(transitionPoints_.end());

    while(++tpIt != tpEnd)
    {
        if(v < tpIt->projected)
        {
            return vigra::NumericTraits<Color>::fromRealPromote(
                prevTP->color + prevTP->scale * (v - prevTP->projected));
        }
        prevTP = tpIt;
    }

    return vigra::NumericTraits<Color>::fromRealPromote(prevTP->color);
}

template<class ITERATOR>
inline void ColorMap::set(ArgumentType v, ITERATOR it) const
{
    *it = operator()(v);
}

#endif // COLORMAP_HXX
