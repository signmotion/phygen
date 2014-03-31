#include "pragma.hcl"
#include "const.hcl"
#include "struct.hcl"
#include "helper.hcl"
#include "image.hcl"

#include "interaction/clay.hcl"
#include "interaction/vacuum.hcl"
#include "interaction/water.hcl"
#include "interaction.hcl"


/**
* �������� �����, ����������� �������������� ��������� � �����������
* ���������� ����� ����.
* ������ ���� = N x M (��. pragma.hcl).
*
* ������������������ ����� 'what' � ���� �� �����. ��������������� ������� ��
* ���������� (��. interaction.cpp): ����� ����� ��� �����������.
*/
__kernel void render(
    // ���������� ����� ����
    // 1 ������� = 4 ������: RGBA, float
    __global float4* imageOut,          // @0
    // ��� ����� ������������ ������� (����, ��������� � �.�.)
    __global Particle* p,               // @1
    // ����� ������������ ������ (������� ����)
    __global WParticle* wp,             // @2
    // ��� ����� ���������?
    const RenderWhat what,              // @3
    // ��������� ����� (����� ���� ������������ � �������)
    const uint randomNumber,            // @4
    // ��������������� �����
    __global WParticle* wpT             // @5
    __global WParticleInteraction* wpI  // @6
) {
    // ������ - ���������� �������
    const uint x = get_global_id( 0 );
    const uint z = get_global_id( 1 );
    // @todo optimize ( N == get_global_size( 0 ) )
    const uint i = x + z * get_global_size( 0 );

    if (what == IMAGE_INTERACTION) {
        // ������ ������������� ���������
        imageWParticle( imageOut, p, wp, i );
        return;
    }
    
    // @test
    //imageOut[i] = (float4)( (float)x / N, (float)z / M, 0.0f, 1.0f );

    switch ( what ) {

        case PREPARE_INTERACTION:
            // ������������ �������� ������� �� ��������� ������ � ��������
            // (!) ��� ����� �������������� � ������� ���������
            calcCharacteristic( wp, p, i );
            return;

        // �������������� ����������� ����.
        // wp -> wpI
        case SELF_INTERACTION:
            selfInteraction( wpI, wp, x, z, i );
            return;

        // ������������ �����������.
        // ����� ��� ������ � SELF_MOVE_INTERACTION, ��� ��������� ��� �����
        // � ������������ � ������� ���� �������.
        // wp -> wpI
        case CORRECT_INTERACTION:
            correctInteraction( wpI, wp, x, z, i );
            return;

        case TEMP_COMMIT_INTERACTION:
            tempCommitInteraction( wp, wpI, i );
            return;

        case COMMIT_INTERACTION:
            wp[i] = wpI[i];
            return;

    }


    // ������������� ���������
    imageWParticle( imageOut, wp, p, i );


    // @test
    /*
    if (i == 0) {
        for (size_t z = 0; z < 10; z++) {
            for (size_t x = 0; x < N; x++) {
                const size_t i = x + z * N;
                //imageOut[i] = (float4)( 1.0f, 1.0f, 1.0f, 1.0f );
                imageOut[i] = (float4)( (float)x / N, (float)z / M, 0.0f, 1.0f );
            }
        }
    }
    */
    /*
    for (size_t x = get_global_id( 0 ); x < N / 2; x += get_global_size( 0 ) ) {
        for (size_t z = get_global_id( 1 ); z < M / 2; z += get_global_size( 1 ) ) {
            const size_t i = x + z * N;
            imageOut[i] = (float4)( (float)x / N, (float)z / M, 0.0f, 1.0f );
        }
    } === [ imageOut[i] = (float4)( (float)x / N, (float)z / M, 0.0f, 1.0f ); ]
    */
    //imageOut[i] = (float4)( 1.0f, 1.0f, 1.0f, 1.0f );
  
    /*
    if (pe[i].mass > 0.0f) {
        imageOut[i] = (float4)( 1.0f, 1.0f, 1.0f, 1.0f );
    } else {
        imageOut[i] = (float4)( 1.0f, 0.0f, 0.0f, 1.0f );
    }
    return;
    */


}
