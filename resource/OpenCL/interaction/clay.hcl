/**
* Глина и Ко.
* wp -> wpT
*
* @see selfInteraction()
*/

inline void clay_ClayToVacuum(
    __global WParticle* wpT,
    __global WParticle* wp,
    const uint iSelf,
    const uint iOther
) {
    // Глина (iSelf) перемещается в вакуум (iOther)
    // См. vacuum_ClayToVacuum()
    wpT[iSelf] = wp[iSelf];
    wpT[iSelf].particle = VACUUM_PARTICLE;

}


/*
inline void clay_WaterToClay(
    __global WParticle* wpT,
    __global WParticle* wp,
    const uint iSelf,
    const uint iOther
) {
    // Вода (iOther) превращает "глину" (iSelf) в "пропитанную водой глину"...
    wpT[iSelf] = wp[iSelf];
    wpT[iSelf].particle = SOGGY_CLAY_PARTICLE;
    
    // ...а сама исчезает.
    // Исчезновение считаем в waterSelfInteraction_WaterToClay().

}
*/





// *****************************************************************
inline void claySelfInteraction(
    __global WParticle* wpT,
    __global WParticle* wp,
    const uint x,
    const uint z,
    const uint i
) {
    wpT[i] = wp[i];

    
    // @test
    {
    const uint iNeighbour = nearCell( x, z, 5 );
    if ( (wp[iNeighbour].particle == VACUUM_PARTICLE) && (wp[i].particle == CLAY_PARTICLE) ) {
        //clay_ClayToVacuum( wpT, wp, i, iNeighbour );
        wpT[i].particle = VACUUM_PARTICLE;
    }
    return;
    }
    

    // I
    // Смотрим направление движения этой частицы
    // Частица неподвижна?
    if (wpT[i].direction == 0) {
        return;
    }

    // Смотрим на соседа, в сторону КОТОРОГО перемещается эта частица
    // и вызываем подходящие методы
    const uint iNeighbour = nearCell( x, z, wp[i].direction );
    switch ( wp[iNeighbour].particle ) {
        case VACUUM_PARTICLE :
            clay_ClayToVacuum( wpT, wp, i, iNeighbour );
            break;

        case CLAY_PARTICLE :
            //clay_ClayToClay( wpT, wp, i, iNeighbour );
            break;

        case WATER_PARTICLE :
            //clay_ClayToWater( wpT, wp, i, iNeighbour );
            break;
    }
    
    return;


    // II
    // Смотрим на соседей, КОТОРЫЕ перемещаются в сторону этой частицы и
    // и вызываем подходящие методы
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
                // Глина не замещает глину
                break;

            case WATER_PARTICLE :
                //clay_WaterToClay( wpT, wp, i, iNeighbour );
                return;
                //break;
        }

    } // for (size_t k = 1; k <= 8; ++k) {


}
