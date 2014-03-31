// (!) Все определения из этого файла должны быть согласованы со struct.h
// @see Примечания в struct.h

enum UIDParticle {
    VACUUM_PARTICLE = 0,
    BASALT_PARTICLE,
    CLAY_PARTICLE,
    SOGGY_CLAY_PARTICLE,
    WATER_PARTICLE,
    COUNT_ALL_PARTICLE
};





// (!) Структуры на HOST и GPU должны быть выровнены.
// @see http://khronos.org/message_boards/viewtopic.php?f=37&t=3098
typedef struct __attribute__ ((packed)) {
    uint particle;

    float mass;
    float velocityX;
    float velocityZ;
    uint direction;
    float temperature;

    float forceX;
    float forceZ;

} WParticle;



typedef struct __attribute__ ((packed)) {
    uchar move;
} WParticleInteraction;




typedef struct __attribute__ ((packed)) {
    uint color;
} Particle;




enum RenderWhat {
    IMAGE_INTERACTION = 0,
    PREPARE_INTERACTION,
    SELF_INTERACTION,
    NEIGHBOUR_INTERACTION,
    TEMP_COMMIT_INTERACTION,
    COMMIT_INTERACTION
};
