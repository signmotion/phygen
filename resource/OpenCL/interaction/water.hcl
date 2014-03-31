/**
* ���� � ��.
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
    // ��������������� ������������� ���� ������� �������
    
    // ���������� ����������� �������� ���� �������.
    // ��������� ������� - ��������, ����� - ����� ��������� ����.
    // �����, ������� ��������� � ����� �����, ���� �� �����������
    // � �������� - ����� (������).
    /*
    if (wp[i].particle == CLAY_PARTICLE) {
        water_WaterToClay( wpT, wp, i, iNeighbour );
        
    } else if (wp[i].particle == VACUUM_PARTICLE) {
        water_WaterToVacuum( wpT, wp, i, iNeighbour );
        
    }
    */


    // ���������� ����������� �������� ������ �� ���� �����.
    // ���� ����� �������� � ���� �������, �������� ����������
    // ������ �����.
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
    // ���� ��������
    wpT[iSelf] = wp[iSelf];
    wpT[iSelf].particle = VACUUM_PARTICLE;
}



inline void water_WaterToVacuum(
    __global WParticle* wpT,
    __global WParticle* wp,
    const uint iSelf,
    const uint iOther
) {
    // ���� ��������
    wpT[iSelf] = wp[iSelf];
    wpT[iSelf].particle = VACUUM_PARTICLE;
}



inline void water_ClayToWater(
    __global WParticle* wpT,
    __global WParticle* wp,
    const uint iSelf,
    const uint iOther
) {
    // ���� ���������� ������ ������
    wpT[iSelf] = wp[iOther];
    wpT[iSelf].particle = SOGGY_CLAY_PARTICLE;
}
