/**
* Формирует визуальный образ мира.
* В индексах WParticle[] зашиты XZ-координаты частиц.
*/
inline void imageWParticle(
    __global float4* imageOut,
    __global Particle* p,
    __global WParticle* wp,
    const uint i
) {
    const uint color = colorParticle( p, wp[i] );

    const float alpha = 1.0f;

    // Цвет переводим в RGBA
    // @source http://rclermont.blogspot.com/2010/02/convert-long-into-byte-array-in-cc.html
    const uchar cr = (int)((color >> 24) & 0xFF);
    const uchar cg = (int)((color >> 16) & 0xFF);
    const uchar cb = (int)((color >> 8) & 0xFF);
    const uchar ca = (int)((color) & 0xFF);

    // Нормализируем каналы
    const float colorDiff = alpha;
    const float r = (float)cr / 255.0f * colorDiff;
    const float g = (float)cg / 255.0f * colorDiff;
    const float b = (float)cb / 255.0f * colorDiff;
    const float a = (float)ca / 255.0f;

    imageOut[i] = (float4)( r, g, b, a );

}

