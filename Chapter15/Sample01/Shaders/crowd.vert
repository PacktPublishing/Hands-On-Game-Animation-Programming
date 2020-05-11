#version 330 core

#define MAX_BONES 60
#define MAX_INSTANCES 80

uniform mat4 view;
uniform mat4 projection;
uniform mat4 invBindPose[MAX_BONES];
uniform sampler2D animTex;

in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec4 weights;
in ivec4 joints;

out vec3 norm;
out vec3 fragPos;
out vec2 uv;

uniform vec3 model_pos[MAX_INSTANCES];
uniform vec4 model_rot[MAX_INSTANCES];
uniform vec3 model_scl[MAX_INSTANCES];

uniform ivec2 frames[MAX_INSTANCES];
uniform float time[MAX_INSTANCES];

vec3 QMulV(vec4 q, vec3 v) {
    return q.xyz * 2.0f * dot(q.xyz, v) +
           v * (q.w * q.w - dot(q.xyz, q.xyz)) +
           cross(q.xyz, v) * 2.0f * q.w;
}

mat4 GetModel(int instance) {
    vec3 position = model_pos[instance];
    vec4 rotation = model_rot[instance];
    vec3 scale = model_scl[instance];

    vec3 xBasis = QMulV(rotation, vec3(scale.x, 0, 0));
    vec3 yBasis = QMulV(rotation, vec3(0, scale.y, 0));
    vec3 zBasis = QMulV(rotation, vec3(0, 0, scale.z));

    return mat4(
        xBasis.x, xBasis.y, xBasis.z, 0.0,
        yBasis.x, yBasis.y, yBasis.z, 0.0,
        zBasis.x, zBasis.y, zBasis.z, 0.0,
        position.x, position.y, position.z, 1.0
    );
}

mat4 GetPose(int joint, int instance) {
    int x_now = frames[instance].x;
    int x_next = frames[instance].y;
    int y_pos = joint * 3;
    
    vec4 pos0 = texelFetch(animTex, ivec2(x_now, (y_pos + 0)), 0);
    vec4 rot0 = texelFetch(animTex, ivec2(x_now, (y_pos + 1)), 0);
    vec4 scl0 = texelFetch(animTex, ivec2(x_now, (y_pos + 2)), 0);

    vec4 pos1 = texelFetch(animTex, ivec2(x_next, (y_pos + 0)), 0);
    vec4 rot1 = texelFetch(animTex, ivec2(x_next, (y_pos + 1)), 0);
    vec4 scl1 = texelFetch(animTex, ivec2(x_next, (y_pos + 2)), 0);

    if (dot(rot0, rot1) < 0.0) { rot1 *= -1.0; }

    vec4 position = mix(pos0, pos1, time[instance]);
    vec4 rotation = normalize(mix(rot0, rot1, time[instance]));
    vec4 scale = mix(scl0, scl1, time[instance]);

    vec3 xBasis = QMulV(rotation, vec3(scale.x, 0, 0));
    vec3 yBasis = QMulV(rotation, vec3(0, scale.y, 0));
    vec3 zBasis = QMulV(rotation, vec3(0, 0, scale.z));

    return mat4(
        xBasis.x, xBasis.y, xBasis.z, 0.0,
        yBasis.x, yBasis.y, yBasis.z, 0.0,
        zBasis.x, zBasis.y, zBasis.z, 0.0,
        position.x, position.y, position.z, 1.0
    );
}

void main() {
    int instance = gl_InstanceID;

    mat4 pose0 = GetPose(joints.x, instance);
    mat4 pose1 = GetPose(joints.y, instance);
    mat4 pose2 = GetPose(joints.z, instance);
    mat4 pose3 = GetPose(joints.w, instance);
    mat4 model = GetModel(instance);

    mat4 skin = (pose0 *  invBindPose[joints.x]) * weights.x;
    skin += (pose1 *  invBindPose[joints.y]) * weights.y;
    skin += (pose2 * invBindPose[joints.z]) * weights.z;
    skin += (pose3 * invBindPose[joints.w]) * weights.w;

    gl_Position = projection * view * model * skin * vec4(position, 1.0);
    fragPos = vec3(model * skin * vec4(position, 1.0));
    norm = vec3(model * skin * vec4(normal, 0.0f));
    uv = texCoord;
}
