// (!) ��� ����������� �� ����� ����� ������ ���� ����������� � OpenCL/struct.h

// @see http://khronos.org/message_boards/viewtopic.php?f=37&t=3098
#pragma pack( push, 16 )



/**
* �������������� ������.
*/
enum UIDParticle {
    // ������
    VACUUM_PARTICLE = 0,
    // �������
    BASALT_PARTICLE,
    // �����
    CLAY_PARTICLE,
    // ����������� ������ �����
    SOGGY_CLAY_PARTICLE,
    // ������� ����
    WATER_PARTICLE,
    // ���������� ��������������� ������
    COUNT_ALL_PARTICLE
};





/**
* ������� �������� � ����.
* ��������, ���������� ��������� '=', �������������� �� ������ ������ �������� �
* �������������, � ������ ������� ��� ��������� ��������.
*
* @see Particle
*/
typedef struct {
    /**
    * �������.
    */
    cl_uint particle;

    /**
    * ����� �������.
    */
    cl_float mass;

    /**
    * = �������� �����������.
    * �������������� ��� ��������� �������� (������� ����� ��) ����
    * ����������� ����.
    */
    cl_float velocityX;
    cl_float velocityZ;

    /**
    * = ����������� ��������.
    * ������� �� ������� ������� � ��������� ����� �������, � �������
    * �������������� �����������.
    * ��������. ������ - ��. helper::near()
    */
    cl_uint direction;

    /**
    * �����������, ������� ��������.
    */
    cl_float temperature;

    /**
    * = ����, ����������� �� �������.
    */
    cl_float forceX;
    cl_float forceZ;

} WParticle;





/**
* ���������� � �������������� ������� � ������� ���������.
* ��������������� ���������. ������� �����. �������� WParticle.
*/
typedef struct {
    /**
    * ����������� ����������� �������.
    * � ����� ������ �� ��������� � WParticle.direction, �.�. � ������
    * ����� ��������� ������ ���� �������.
    * 0 - ������� ������� �� ���� �����.
    *
    * @see helper:: near()
    */
    cl_uchar move;

} WParticleInteraction;





/**
* ����� �������� �������.
*
* @see WParticle
*/
typedef struct {
    /**
    * ������������� �������.
    *//* - ����� ��� ����� �/� � ������; �����. enum UIDParticle.
    cl_uint uid;
    */

    /**
    * ���� �������.
    */
    cl_uint color;

} Particle;






/**
* ��� ����� ���������?
*/
enum RenderWhat {
    // ������������ ���������� �����
    // ����� - ����� ������
    IMAGE_INTERACTION = 0,
    // ����, ����������� ��� � ������ ���������
    // ���������� ������
    PREPARE_INTERACTION,
    // ���� ������� ��������������� � �������
    SELF_INTERACTION,
    // �������-����� ��������������� � ����� ��������
    NEIGHBOUR_INTERACTION,
    // ����������� ���������
    TEMP_COMMIT_INTERACTION,
    COMMIT_INTERACTION
};
