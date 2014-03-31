/**
* Взаимодействие частиц.
* Характеристики для частиц уже должны быть посчитаны.
*/



/**
* Вычисляем воздействие на саму частицу других частиц.
* result -> wpI
*/
inline void selfInteraction(
    __global WParticle* wpI,
    __global WParticle* wp,
    const uint x,
    const uint z,
    // = x + z * N
    const uint i
) {
    // @test
    wpI[i].move = 1;
    return;
    
    

    // Для каждой частицы созданы методы, декларирующие её поведение
    // относительно других частиц (не обязательно соседних).
    // Частица может изменить не только своё состояние, но и состояние
    // других частиц. Например, вода, соприкоснувшись с глиной, может
    // образовать "пропитанную водой глину".
    // В методах АБInteraction() прописано, как частица А воздействует
    // на частицу Б.

    // Методы сами извлекают информацию для частицы. Например:
    //   - кто находится по соседству
    //   - в каком направлении перемещаются соседи
    //   - температура соседей
    //   - ...

    // Вызываем подходящие методы
    switch ( wp[i].particle ) {
        case VACUUM_PARTICLE :
            vacuumSelfInteraction( wpI, wp, x, z, i );
            break;

        case CLAY_PARTICLE :
            claySelfInteraction( wpI, wp, x, z, i );
            break;

        case WATER_PARTICLE :
            //waterSelfInteraction( wpI, wp, x, z, i );
            break;
        
        default:
            //throw "Not defined particle.";
            wpI[i].move = 0;
    }
    
}
