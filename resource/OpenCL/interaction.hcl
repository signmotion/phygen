/**
* �������������� ������.
* �������������� ��� ������ ��� ������ ���� ���������.
*/



/**
* ��������� ����������� �� ���� ������� ������ ������.
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
    
    

    // ��� ������ ������� ������� ������, ������������� � ���������
    // ������������ ������ ������ (�� ����������� ��������).
    // ������� ����� �������� �� ������ ��� ���������, �� � ���������
    // ������ ������. ��������, ����, ��������������� � ������, �����
    // ���������� "����������� ����� �����".
    // � ������� ��Interaction() ���������, ��� ������� � ������������
    // �� ������� �.

    // ������ ���� ��������� ���������� ��� �������. ��������:
    //   - ��� ��������� �� ���������
    //   - � ����� ����������� ������������ ������
    //   - ����������� �������
    //   - ...

    // �������� ���������� ������
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
