class ColorMap /Abstract/
{
%TypeHeaderCode
#include <VigraQt/colormap.hxx>
%End

  public:
    typedef vigra::RGBValue<unsigned char> Color;
    typedef float ArgumentType;

    typedef ArgumentType argument_type;
    typedef Color result_type;

    virtual ~ColorMap();

    virtual void setDomain(ArgumentType min, ArgumentType max) = 0;

    virtual ArgumentType domainMin() const = 0;

    virtual ArgumentType domainMax() const = 0;

        //    virtual Color operator()(ArgumentType v) const = 0;
};

enum BuiltinColorMap
{
    CMGray, // enhanced gray map (uses color variations of gray)
    CMLinearGray, // LinearColorMap instance w/ black..white gradient
    CMFire,
    CMFireNegativeBlue,
};

ColorMap *createColorMap(BuiltinColorMap cm);
