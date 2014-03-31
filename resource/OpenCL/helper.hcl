// ��������������� ������.


/**
* ��������� �������� � �������� ���������.
*/
__constant const float PRECISION_POSITIVE = 1e-5f;
__constant const float PRECISION_NEGATIVE = -1e-5f;

inline bool zero(float a) {
    return  ( isless( a, PRECISION_POSITIVE ) && isgreater( a, PRECISION_NEGATIVE ) );
}

inline bool lZero(float a) {
    return isless( a, PRECISION_NEGATIVE );
}

inline bool leZero(float a) {
    return lZero( a ) || zero( a );
}

inline bool gZero(float a) {
    return isgreater( a, PRECISION_POSITIVE );
}

inline bool geZero(float a) {
    return gZero( a ) || zero( a );
}

inline bool equal(float a, float b) {
    return zero( a - b );
}

inline bool positiveOne(float a) {
    return zero( a - 1 );
}

inline bool negativeOne(float a) {
    return zero( a + 1 );
}

inline bool one(float a) {
    return positiveOne( a ) || negativeOne( a );
}





/**
* @return �������, ��� ��������� ����� ����� � �����. �������� ������
* ������� ������� N x M. N, M �������� � pragma.hcl
*/
inline bool borderRight( const uint x, const uint z ) {
    return (x == (N - 1));
}

inline bool borderBottom( const uint x, const uint z ) {
    return (z == 0);
}

inline bool borderLeft( const uint x, const uint z ) {
    return (x == 0);
}

inline bool borderTop( const uint x, const uint z ) {
    return (z == (M - 1));
}


/**
* @return �������, ��� ��������� ����� ����� ������ ������� N x M.
* == !outsideMatrix()
*/
inline bool insideMatrix( const int x, const int z ) {
    return (
        (x >= 0) && (x < N)
     && (z >= 0) && (z < M)
    );
}



/**
* @return �������, ��� ��������� ����� ����� ��� ������� N x M.
* == !insideMatrix()
*/
inline bool outsideMatrix( const int x, const int z ) {
    return (
        (z < 0) || (z >= M)
     || (x < 0) || (x >= N)
    );
}




/**
* @return �������, ��� ������� ������������ ����� ������.
*/
inline bool vacuum( const WParticle wp ) {
    return (wp.particle == VACUUM_PARTICLE);
}





/**
* @return ���������� ������ ��������� (������������ �������) ������.
*         ���� ������ ������� �� ������� ����, ���������� -1.
*
* ������������ �����:
*   6   7   8   .   .   N
*   5   0   1
*   4   3   2
*   .
*   .
*   M
* 
*/
inline int near(
    const int x, const int z,
    const int dx, const int dz
) {
    return (
        ( outsideMatrix( x + dx, z + dz ) || outsideMatrix( x, z ) )
          ? -1
          : (x + dx) + (z + dz) * N
    );
}



/**
* @see near()
*/
inline int nearCell(
    const int x, const int z,
    const uint cell
) {
    switch ( cell ) {
        case 1:  return near( x, z, +1, 0 );
        case 2:  return near( x, z, +1, -1 );
        case 3:  return near( x, z, 0, -1 );
        case 4:  return near( x, z, -1, -1 );
        case 5:  return near( x, z, -1, 0 );
        case 6:  return near( x, z, -1, +1 );
        case 7:  return near( x, z, 0, +1 );
        case 8:  return near( x, z, +1, +1 );
        case 0:  return near( x, z, 0, 0 );
    }

    return -1;
}





/**
* ����������, � ����� ������� ������������ ������� ��������� ����� ��������.
* ���������� ����� �������. ����������� 8-�� �������.
* '0' ��������, ��� ������� ����������.
*
* @see ��������� ����� � near()
*/
inline size_t transfer(
    const float2 velocity
) {
    // @test
    return 1;

#if 0
    if ( zero( velocity.x ) && zero( velocity.y ) ) {
        return 0;
    }

    // ����������� ��������
    // @todo optimize ���� � OpenCL ������� �������?
    const bool negativeX = signbit( velocity.x );
    const bool negativeZ = signbit( velocity.y );
    const float vAbsMax = max(
        select( velocity.x, -velocity.x, negativeX ),
        select( velocity.y, -velocity.y, negativeZ )
    );
    const float2 vn = (float2)( velocity.x / vAbsMax, velocity.y / vAbsMax );
    
    /*
    // ���������� ������������ ���������� (== 1.0)
    const bool xGreater = isgreater( velocity.x, velocity.y );
    const float �Max = select( velocity.y, velocity.x, xGreater );
    const float cMin = select( velocity.x, velocity.y, xGreater );
    */

    // ����� ����� ���������, ��� ���� �� �������� - �� ������� (��. ����)
    const bool positiveX = !negativeX;
    const bool positiveZ = !negativeZ;
    
    if ( zero( vn.x ) ) {
        //return select( 3, 7, positiveZ );
        return positiveZ ? 7 : 3;
    }
    
    if ( zero( vn.y ) ) {
        //return select( 5, 1, positiveX );
        return positiveX ? 1 : 5;
    }
    
    
    const bool oneX = one( vn.x );
    const bool oneZ = one( vn.y );

    float c1 = 0.0f;
    float c2 = 0.0f;
    float c3 = 0.0f;
    float c4 = 0.0f;
    float c5 = 0.0f;
    float c6 = 0.0f;
    float c7 = 0.0f;
    float c8 = 0.0f;
    
    if ( oneX ) {
        // ������� ����
        // 2
        if ( positiveX && negativeZ ) {
            c2 = -vn.y;
        }
        // 4
        if ( negativeX && negativeZ ) {
            c4 = -vn.y;
        }
        // 6
        if ( negativeX && positiveZ ) {
            c6 = vn.y;
        }
        // 8
        if ( positiveX && positiveZ ) {
            c8 = vn.y;
        }

        // ������� ����
        // 1
        if ( positiveX ) {
            c1 = vn.x - c8 - c2;
        }
        // 3
        /* - ������ == 0
        if ( negativeZ ) {
            c3 = -vn.y - c2 - c4;
        }
        */
        // 5
        if ( positiveX ) {
            c5 = -vn.x - c4 - c6;
        }
        // 7
        /* - ������ == 0
        if ( negativeZ ) {
            c7 =  vn.y - c6 - c8;
        }
        */
        
    } else if ( oneZ ) {
        // ������� ����
        // 2
        if ( positiveX && negativeZ ) {
            c2 = vn.x;
        }
        // 4
        if ( negativeX && negativeZ ) {
            c4 = -vn.x;
        }
        // 6
        if ( negativeX && positiveZ ) {
            c6 = -vn.x;
        }
        // 8
        if ( positiveX && positiveZ ) {
            c8 = vn.x;
        }

        // ������� ����
        // 1
        /* - ������ == 0
        if ( positiveX ) {
        }
        */
        // 3
        if ( negativeZ ) {
            c3 = -vn.y - c2 - c4;
        }
        // 5
        /* - ������ == 0
        if ( positiveX ) {
        }
        */
        // 7
        if ( negativeZ ) {
            c7 = vn.y - c6 - c8;
        }

    } // else if ( oneZ ) {
    
    
    // ������� ������������ � �������, � ������� �������� �������
    // ����������� ��������
    size_t d = 0;
    float c = 0.0f;
    if ( isgreater( c1, c ) )  { d = 1; c = c1; }
    if ( isgreater( c2, c ) )  { d = 2; c = c2; }
    if ( isgreater( c3, c ) )  { d = 3; c = c3; }
    if ( isgreater( c4, c ) )  { d = 4; c = c4; }
    if ( isgreater( c5, c ) )  { d = 5; c = c5; }
    if ( isgreater( c6, c ) )  { d = 6; c = c6; }
    if ( isgreater( c7, c ) )  { d = 7; c = c7; }
    if ( isgreater( c8, c ) )  { d = 8; c = c8; }
    
    return d;
#endif
}






/**
* @return ���� �������.
*/
inline uint colorParticle(
    __global Particle* p,
    const WParticle wp
) {
    return p[ wp.particle ].color;
}




/**
* ������������ �������������� ��� ���������� �������.
*
* @see struct WParticle
*/
inline void calcCharacteristic(
    __global Particle* p,
    __global WParticle* wp,
    const uint i
) {
    if ( vacuum( wp[i] ) ) {
        // ��� ������� ������� ������
        return;
    }

    // ��������� ��������������
    
    // �������� �����������
    const float forceX = wp[i].forceX + DEFAULT_FORCE_X;
    const float forceZ = wp[i].forceZ + DEFAULT_FORCE_Z;
    const bool zeroMass = zero( wp[i].mass );
    const float accelerationX = zeroMass ? 0.0f : ( forceX / wp[i].mass );
    const float accelerationZ = zeroMass ? 0.0f : ( forceZ / wp[i].mass );
    wp[i].velocityX += accelerationX;
    wp[i].velocityZ += accelerationZ;
    
    // ����������� ��������
    const float2 velocity = (float2)( wp[i].velocityX, wp[i].velocityZ );
    wp[i].direction = transfer( velocity );
}
