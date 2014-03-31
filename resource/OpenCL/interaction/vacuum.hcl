/**
* ������ � ��.
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
    // ������ (iSelf) ��������� ����� (iOther)
    // ��. clay_ClayToVacuum()
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
    // ������ �� � ��� �� ���������������, �� ���������
    // � ���� ������ ������ �������
    wpI[i].move = 0;
}


    


/**
* ����������� ������ � ������� ���� ������.
* ������� wpI ��� ������ ���� ���������.
*/
inline void vacuumNeighbourInteraction(
    __global WParticleInteraction* wpI,
    __global WParticle* wp,
    const uint x,
    const uint z,
    const uint i
) {
    // ������ �� � ��� �� ���������������, �� ���������
    // � ���� ������ ������ �������
    wpI[i].move = 0;


    // I
    // ������ �� ������������
    

    // II
    // ������� �� �������, ������� ������������ � ������� ���� ������� �
    // � �������� ���������� ������
    
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
        // ���������� ����������� �������� ����� ������
        // ����� ����������?
        if (wp[iNeighbour].direction == 0) {
            continue;
        }

        // ������������ ����� ������ ������ ���������� �����
        switch ( wp[iNeighbour].particle ) {
            case VACUUM_PARTICLE :
                // ������ �� ������������
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
