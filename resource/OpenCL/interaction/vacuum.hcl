/**
* Вакуум и Ко.
* result -> wpI
*
* @see selfInteraction()
*/

inline void vacuum_ClayToVacuum(
    __global WParticleInteraction* wpI,
    __global WParticle* wp,
    const uint iSelf,
    const uint iOther
) {
    // Вакуум (iSelf) принимает глину (iOther)
    // См. clay_ClayToVacuum()
    wpT[iSelf] = wp[iSelf];
    wpT[iSelf].particle = CLAY_PARTICLE;

}





inline void vacuumSelfInteraction(
    __global WParticleInteraction* wpI,
    __global WParticle* wp,
    const uint x,
    const uint z,
    const uint i
) {
    // Вакуум ни с кем не взаимодействует, но принимает
    // в свою ячейку разные материи
    wpI[i].move = 0;
}


    


/**
* Перемещение соседа в сторону этой ячейки.
* Полотно wpI уже должно быть заполнено.
*/
inline void vacuumNeighbourInteraction(
    __global WParticleInteraction* wpI,
    __global WParticle* wp,
    const uint x,
    const uint z,
    const uint i
) {
    // Вакуум ни с кем не взаимодействует, но принимает
    // в свою ячейку разные материи
    wpI[i].move = 0;


    // I
    // Вакуум не перемещается
    

    // II
    // Смотрим на соседей, КОТОРЫЕ перемещаются в сторону этой частицы и
    // и вызываем подходящие методы
    
    // @test
    {
    const uint iNeighbour = nearCell( x, z, 1 );
    if (wp[iNeighbour].particle == CLAY_PARTICLE) {
        //vacuum_ClayToVacuum( wpT, wp, i, iNeighbour );
        wpT[i].particle = CLAY_PARTICLE;
    }
    return;
    }

    
    for (size_t k = 1; k <= 8; ++k) {
        const uint iNeighbour = nearCell( x, z, k );
        // Определяем направление движения этого соседа
        // Сосед неподвижен?
        if (wp[iNeighbour].direction == 0) {
            continue;
        }

        // Перемещаться будет только первый подходящий сосед
        switch ( wp[iNeighbour].particle ) {
            case VACUUM_PARTICLE :
                // Вакуум не перемещается
                break;

            case CLAY_PARTICLE :
                vacuum_ClayToVacuum( wpT, wp, i, iNeighbour );
                return;
                //break;

            case WATER_PARTICLE :
                //vacuum_WaterToVacuum( wpT, wp, i, iNeighbour );
                return;
                //break;
        }

    } // for (size_t k = 1; k <= 8; ++k) {

}
