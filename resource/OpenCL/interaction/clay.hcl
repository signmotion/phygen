/**
* ����� � ��.
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
    // ����� (iSelf) ������������ � ������ (iOther)
    // ��. vacuum_ClayToVacuum()
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
    // ���� (iOther) ���������� "�����" (iSelf) � "����������� ����� �����"...
    wpT[iSelf] = wp[iSelf];
    wpT[iSelf].particle = SOGGY_CLAY_PARTICLE;
    
    // ...� ���� ��������.
    // ������������ ������� � waterSelfInteraction_WaterToClay().

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
    // ������� ����������� �������� ���� �������
    // ������� ����������?
    if (wpT[i].direction == 0) {
        return;
    }

    // ������� �� ������, � ������� �������� ������������ ��� �������
    // � �������� ���������� ������
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
    // ������� �� �������, ������� ������������ � ������� ���� ������� �
    // � �������� ���������� ������
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
                // ����� �� �������� �����
                break;

            case WATER_PARTICLE :
                //clay_WaterToClay( wpT, wp, i, iNeighbour );
                return;
                //break;
        }

    } // for (size_t k = 1; k <= 8; ++k) {


}
