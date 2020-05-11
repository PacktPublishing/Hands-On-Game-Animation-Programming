#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec4 weights;
in ivec4 joints;

uniform mat2x4 pose[120];
uniform mat2x4 invBindPose[120];

out vec3 norm;
out vec3 fragPos;
out vec2 uv;

vec4 mulQ(vec4 Q1, vec4 Q2) {
    return vec4(
         Q2.x * Q1.w + Q2.y * Q1.z - Q2.z * Q1.y + Q2.w * Q1.x,
        -Q2.x * Q1.z + Q2.y * Q1.w + Q2.z * Q1.x + Q2.w * Q1.y,
         Q2.x * Q1.y - Q2.y * Q1.x + Q2.z * Q1.w + Q2.w * Q1.z,
        -Q2.x * Q1.x - Q2.y * Q1.y - Q2.z * Q1.z + Q2.w * Q1.w
    );
}

mat2x4 normalizeDq(mat2x4 dq) {
    float invMag = 1.0 / length(dq[0]);
    dq[0] *= invMag;
    dq[1] *= invMag;
    return dq;
}

mat2x4 combineDq(mat2x4 l, mat2x4 r) {
    l = normalizeDq(l);
    r = normalizeDq(r);

    vec4 real = mulQ(l[0], r[0]);
    vec4 dual = mulQ(l[0], r[1]) + mulQ(l[1], r[0]);

    return mat2x4(real, dual);
}

vec4 transformVector(mat2x4 dq, vec3 v) {
    vec4 real = dq[0];
    vec3 r_vector = real.xyz;
    float r_scalar = real.w;
    
    vec3 rotated = r_vector * 2.0f * dot(r_vector, v) +
            v * (r_scalar * r_scalar - dot(r_vector, r_vector)) +
            cross(r_vector, v) * 2.0f * r_scalar;

    return vec4(rotated, 0);
}

vec4 transformPoint(mat2x4 dq, vec3 v) {
    vec4 real = dq[0];
    vec4 dual = dq[1];

    vec3 rotated = transformVector(dq, v).xyz;
    vec4 conjugate = vec4(-real.xyz, real.w);
    vec3 t = mulQ(conjugate, dual * 2.0).xyz;
    
    return vec4(rotated + t, 1);
}

void main() {
    vec4 w = weights;

    // Neighborhood all of the quaternions correctly
    if (dot(pose[joints.x][0], pose[joints.y][0]) < 0.0) { w.y *= -1.0; }
    if (dot(pose[joints.x][0], pose[joints.z][0]) < 0.0) { w.z *= -1.0; }
    if (dot(pose[joints.x][0], pose[joints.w][0]) < 0.0) { w.w *= -1.0; }

	// Combine 
	mat2x4 dq0 = combineDq(invBindPose[joints.x], pose[joints.x]);
    mat2x4 dq1 = combineDq(invBindPose[joints.y], pose[joints.y]);
    mat2x4 dq2 = combineDq(invBindPose[joints.z], pose[joints.z]);
    mat2x4 dq3 = combineDq(invBindPose[joints.w], pose[joints.w]);

	mat2x4 skinDq = w.x * dq0 + w.y * dq1 + w.z * dq2 +  w.w * dq3;
    skinDq = normalizeDq(skinDq);

	vec4 v = transformPoint(skinDq, position);
	gl_Position = projection * view * model * v;
	fragPos = vec3(model * v);
	
	vec4 n = transformVector(skinDq, normal);
	norm = vec3(model * n);
	uv = texCoord;
}
