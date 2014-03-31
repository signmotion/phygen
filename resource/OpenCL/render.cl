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
* Основной метод, вычисляющий взаимодействия сущностей и формирующий
* визуальный образ мира.
* Размер мира = N x M (см. pragma.hcl).
*
* Последовательность путей 'what' в коде не соотв. действительному порядку их
* выполнения (см. interaction.cpp): задан таким для оптимизации.
*/
__kernel void render(
    // Визуальный образ мира
    // 1 пиксель = 4 канала: RGBA, float
    __global float4* imageOut,          // @0
    // Что собой представляют частицы (цвет, плотность и т.д.)
    __global Particle* p,               // @1
    // Карта расположения частиц (полотно мира)
    __global WParticle* wp,             // @2
    // Что будем вычислять?
    const RenderWhat what,              // @3
    // Случайное число (может быть использовано в методах)
    const uint randomNumber,            // @4
    // Вспомогательные карты
    __global WParticle* wpT             // @5
    __global WParticleInteraction* wpI  // @6
) {
    // Всегда - координаты частицы
    const uint x = get_global_id( 0 );
    const uint z = get_global_id( 1 );
    // @todo optimize ( N == get_global_size( 0 ) )
    const uint i = x + z * get_global_size( 0 );

    if (what == IMAGE_INTERACTION) {
        // Только визуализируем результат
        imageWParticle( imageOut, p, wp, i );
        return;
    }
    
    // @test
    //imageOut[i] = (float4)( (float)x / N, (float)z / M, 0.0f, 1.0f );

    switch ( what ) {

        case PREPARE_INTERACTION:
            // Рассчитываем значения частицы на основании других её значений
            // (!) без учёта взаимодействия с другими частицами
            calcCharacteristic( wp, p, i );
            return;

        // Подготавливаем перемещение себя.
        // wp -> wpI
        case SELF_INTERACTION:
            selfInteraction( wpI, wp, x, z, i );
            return;

        // Корректируем перемещения.
        // Сосед уже сказал в SELF_MOVE_INTERACTION, что освободит своё место
        // и переместится в сторону этой частицы.
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


    // Визуализируем результат
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
