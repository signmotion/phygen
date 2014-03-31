/**
* Физические константы.
*
* @see http://en.wikipedia.org/wiki/Physical_constant
*/
namespace physical { namespace constant {

#ifdef LOW_PRECISION_PHYSICAL_CONSTANT
    typedef float value_t;
#else
    typedef double value_t;
#endif


/**
* Минимально возможная температура, Кельвин.
* @source http://ru.wikipedia.org/wiki/%D0%95%D0%B4%D0%B8%D0%BD%D0%B8%D1%86%D1%8B_%D0%B8%D0%B7%D0%BC%D0%B5%D1%80%D0%B5%D0%BD%D0%B8%D1%8F_%D1%82%D0%B5%D0%BC%D0%BF%D0%B5%D1%80%D0%B0%D1%82%D1%83%D1%80%D1%8B
*/
const auto MIN_TEMPERATURE_KELVIN = (value_t)0;
const auto MIN_TEMPERATURE_CELSIUS = (value_t)-273.15;


/**
* Максимально возможная температура, Кельвин.
* @source http://ru.wikipedia.org/wiki/%D0%9F%D0%BB%D0%B0%D0%BD%D0%BA%D0%BE%D0%B2%D1%81%D0%BA%D0%B0%D1%8F_%D1%82%D0%B5%D0%BC%D0%BF%D0%B5%D1%80%D0%B0%D1%82%D1%83%D1%80%D0%B0
*/
const auto MAX_TEMPERATURE_KELVIN = (value_t)1.41679e+32;
const auto MAX_TEMPERATURE_CELSIUS = MAX_TEMPERATURE_KELVIN + MIN_TEMPERATURE_CELSIUS;


/**
* Газовая постоянная, Дж / (Моль * K)
* @source http://ru.wikipedia.org/wiki/%D0%A3%D0%BD%D0%B8%D0%B2%D0%B5%D1%80%D1%81%D0%B0%D0%BB%D1%8C%D0%BD%D0%B0%D1%8F_%D0%B3%D0%B0%D0%B7%D0%BE%D0%B2%D0%B0%D1%8F_%D0%BF%D0%BE%D1%81%D1%82%D0%BE%D1%8F%D0%BD%D0%BD%D0%B0%D1%8F
*/ 
const auto GAS_CONSTANT = (value_t)8.3144621;





/**
* Коэффициенты для перевода величин.
*/
namespace correlation {


/**
* Градусы Цельсия в Кельвины.
*/
const auto CELSIUS_KELVIN_TEMPERATURE = MIN_TEMPERATURE_CELSIUS;


} // namespace correlation {






} } // namespace physical { namespace constant {
