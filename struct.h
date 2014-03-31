// (!) Все определения из этого файла должны быть согласованы с OpenCL/struct.h

// @see http://khronos.org/message_boards/viewtopic.php?f=37&t=3098
#pragma pack( push, 16 )



/**
* Идентификаторы частиц.
*/
enum UIDParticle {
    // Вакуум
    VACUUM_PARTICLE = 0,
    // Базальт
    BASALT_PARTICLE,
    // Глина
    CLAY_PARTICLE,
    // Пропитанная влагой глина
    SOGGY_CLAY_PARTICLE,
    // Частица воды
    WATER_PARTICLE,
    // Количество декларированных частиц
    COUNT_ALL_PARTICLE
};





/**
* Частица сущности в мире.
* Значения, отмеченные префиксом '=', рассчитываются на основе других значений и
* предназначены, в первую очередь для ускорения расчётов.
*
* @see Particle
*/
typedef struct {
    /**
    * Частица.
    */
    cl_uint particle;

    /**
    * Масса частицы.
    */
    cl_float mass;

    /**
    * = Скорость перемещения.
    * Рассчитывается как начальная скорость (указана здесь же) плюс
    * воздействие силы.
    */
    cl_float velocityX;
    cl_float velocityZ;

    /**
    * = Направление движения.
    * Зависит от вектора стороны и указывает номер стороны, в которую
    * осуществляется перемещение.
    * Нумераци. сторон - см. helper::near()
    */
    cl_uint direction;

    /**
    * Температура, градусы Кельвина.
    */
    cl_float temperature;

    /**
    * = Сила, действующая на частицу.
    */
    cl_float forceX;
    cl_float forceZ;

} WParticle;





/**
* Информация о взаимодействии частицы с другими частицами.
* Вспомогательная структура. Размеры соотв. размерам WParticle.
*/
typedef struct {
    /**
    * Направление перемещения частицы.
    * В общем случае не совпадает с WParticle.direction, т.к. в ячейке
    * может находится только одна частица.
    * 0 - Частица остаётся на своём месте.
    *
    * @see helper:: near()
    */
    cl_uchar move;

} WParticleInteraction;





/**
* Общее описание частицы.
*
* @see WParticle
*/
typedef struct {
    /**
    * Идентификатор частицы.
    *//* - Зашит как номер п/п в списке; соотв. enum UIDParticle.
    cl_uint uid;
    */

    /**
    * Цвет частицы.
    */
    cl_uint color;

} Particle;






/**
* Что будем рендерить?
*/
enum RenderWhat {
    // Сформировать визуальный образ
    // Образ - карта частиц
    IMAGE_INTERACTION = 0,
    // Шаги, переводящие мир в другое состояние
    // Подготовка данных
    PREPARE_INTERACTION,
    // Сама частица взаимодействует с соседом
    SELF_INTERACTION,
    // Частица-сосед взаимодействует с самой частицей
    NEIGHBOUR_INTERACTION,
    // Закрепление изменений
    TEMP_COMMIT_INTERACTION,
    COMMIT_INTERACTION
};
