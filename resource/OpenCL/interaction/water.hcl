/**
* Вода и Ко.
* result -> wpI
*
* @see selfInteraction()
*/

inline void waterSelfInteraction(
    __global WParticleInteraction* wpI,
    __global WParticle* wp,
    const uint x,
    const uint z,
    const uint i
) {
    // Последовательно просматриваем всех соседей частицы
    
    // Определяем направление движения этой частицы.
    // Некоторый частицы - например, глина - могут поглотить воду.
    // Также, частица пропадает с этого места, если по направлению
    // её движения - пусто (вакуум).
    /*
    if (wp[i].particle == CLAY_PARTICLE) {
        water_WaterToClay( wpT, wp, i, iNeighbour );
        
    } else if (wp[i].particle == VACUUM_PARTICLE) {
        water_WaterToVacuum( wpT, wp, i, iNeighbour );
        
    }
    */


    // Определяем направление движения соседа на этом такте.
    // Если сосед движется в нашу сторону, вызываем подходящий
    // случаю метод.
    /*
    if (wp[iNeighbour].particle == CLAY_PARTICLE) {
        water_ClayToWater( wpT, wp, i, iNeighbour );
    }
    */

}




inline void water_WaterToClay(
    __global WParticle* wpT,
    __global WParticle* wp,
    const uint iSelf,
    const uint iOther
) {
    // Вода исчезает
    wpT[iSelf] = wp[iSelf];
    wpT[iSelf].particle = VACUUM_PARTICLE;
}



inline void water_WaterToVacuum(
    __global WParticle* wpT,
    __global WParticle* wp,
    const uint iSelf,
    const uint iOther
) {
    // Вода исчезает
    wpT[iSelf] = wp[iSelf];
    wpT[iSelf].particle = VACUUM_PARTICLE;
}



inline void water_ClayToWater(
    __global WParticle* wpT,
    __global WParticle* wp,
    const uint iSelf,
    const uint iOther
) {
    // Вода заменяется мокрой глиной
    wpT[iSelf] = wp[iOther];
    wpT[iSelf].particle = SOGGY_CLAY_PARTICLE;
}
