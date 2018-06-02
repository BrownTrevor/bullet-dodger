#pragma once
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

class keyframe
	{
	public:
		quat quaternion;
		vec3 translation;
		long long timestamp_ms;
	};
class animation_per_bone
	{
	public:
		string name;
		long long duration;
		int frames;
		string bone;
		vector<keyframe> keyframes;
	};
class all_animations
	{
	public:
		vector<animation_per_bone> animations;

	};


//**************************************************

class bone
{
public:
	vector<animation_per_bone*> animation;	//all the animations of the bone
	string name;
	vec3 pos;
	quat q;
	bone *parent = NULL;
	vector<bone*> kids;			
	unsigned int index;			//a unique number for each bone, at the same time index of the animatiom matrix array
	mat4 *mat = NULL;			//address of one lement from the animation matrix array
	// searches for the animation and sets the animation matrix element to the recent matrix gained from the keyframe
	void play_animation(float* keyframenumber, int ai1, int ai2, float t)
	{
		float kfn1;
		float kfn2;
		if (animation[ai1]->keyframes.size() == animation[ai2]->keyframes.size()) {
			kfn1 = *keyframenumber;
			kfn2 = *keyframenumber;
		}
		else {
			float avgkfn = (animation[ai1]->keyframes.size() + animation[ai2]->keyframes.size()) /2;
			//cast part to float 
			if (t < .000001) {
				kfn1 = (*keyframenumber * (animation[ai1]->keyframes.size() / animation[ai1]->keyframes.size()));
				kfn2 = (*keyframenumber * (animation[ai2]->keyframes.size() / animation[ai1]->keyframes.size()));
			}
			else if (t-1 < .000001){
				kfn1 = (*keyframenumber * (animation[ai1]->keyframes.size() / animation[ai2]->keyframes.size()));
				kfn2 = (*keyframenumber * (animation[ai2]->keyframes.size() / animation[ai2]->keyframes.size()));
			}
			else {
				kfn1 = (*keyframenumber * (animation[ai1]->keyframes.size() / avgkfn));
				kfn2 = (*keyframenumber * (animation[ai2]->keyframes.size() / avgkfn));
			}
		}
		quat q;
		vec3 tr;
		if (!(animation[ai1]->keyframes.size() -2 > (int)kfn1 && animation[ai2]->keyframes.size() > (int)kfn2)) {
			*keyframenumber = 0;
			kfn1 = 0;
			kfn2 = 0;
		}
		if (abs(t - 1) < .001 || t < .001) { 
			float kf = kfn1;
			int i = ai1;
			if (abs(t - 1) < .001) {
				i = ai2;
				kf = kfn2;
			}
			quat p = animation[i]->keyframes[(int)kf].quaternion;
			quat c = animation[i]->keyframes[((int)kf + 1) % animation[i]->keyframes.size()].quaternion;
			q = slerp(p, c, kf - (long)kf);
			tr = (1 - (kf - (long)kf)) * animation[i]->keyframes[kf].translation 
				+ (kf - (long)kf) * animation[i]->keyframes[((int)kf + 1) % animation[i]->keyframes.size()].translation;
		}
		else {
			quat p = animation[ai1]->keyframes[(int)kfn1].quaternion;
			quat c = animation[ai1]->keyframes[((int)kfn1 + 1) % animation[ai1]->keyframes.size()].quaternion;
			quat q1 = slerp(p,c,kfn1 - long(kfn1));
			p = animation[ai2]->keyframes[(int)kfn2].quaternion;
			c = animation[ai2]->keyframes[((int)kfn2 + 1) % animation[ai2]->keyframes.size()].quaternion;
			quat q2 = slerp(p, c, kfn2 - long(kfn2)); 
			q = slerp(q1, q2, t);
			q = normalize(q);
			vec3 tr1 = (1 - (kfn1 - (long)kfn1)) * animation[ai1]->keyframes[kfn1].translation
				+ (kfn1 - (long)kfn1) * animation[ai1]->keyframes[((int)kfn1 + 1) % animation[ai1]->keyframes.size()].translation;
			vec3 tr2 = (1 - (kfn2 - (long)kfn2)) * animation[ai2]->keyframes[kfn2].translation
				+ (kfn2 - (long)kfn2) * animation[ai2]->keyframes[((int)kfn2 + 1) % animation[ai2]->keyframes.size()].translation;
			tr = (1 - t) * tr1 + t * tr2;
			//tr = (1 - t) * animation[ai1]->keyframes[kfn1].translation + t * animation[ai2]->keyframes[kfn2].translation;
		}
		if (name == "Humanoid:Hips")
			tr = vec3(0, 0, 0);
		mat4 M = mat4(q);
		mat4 T = translate(mat4(1), tr);
		M = T * M;
		if (mat)
		{
			mat4 parentmat = mat4(1);
			if (parent)
				parentmat = *parent->mat;
			*mat = parentmat * M;
		}
		else {
			*mat = mat4(1);
		}
		for (int i = 0; i < kids.size(); i++)
			kids[i]->play_animation(keyframenumber, ai1, ai2, t);

		
	}


		
	//writes into the segment positions and into the animation index VBO
	void write_to_VBOs(vec3 origin, vector<vec3> &vpos, vector<unsigned int> &imat)
		{
		vpos.push_back(origin);
		vec3 endp = origin + pos;
		vpos.push_back(endp);

		if(parent)
			imat.push_back(parent->index);
		else
			imat.push_back(index);
		imat.push_back(index);

		for (int i = 0; i < kids.size(); i++)
			kids[i]->write_to_VBOs(endp, vpos, imat);
		}
	//searches for the correct animations as well as sets the correct element from the animation matrix array
	void set_animations(all_animations *all_anim,mat4 *matrices,int &animsize)
		{
		for (int ii = 0; ii < all_anim->animations.size(); ii++) {
			if (all_anim->animations[ii].bone == name)
				animation.push_back(&all_anim->animations[ii]);
		}

		mat = &matrices[index];
		animsize++;

		for (int i = 0; i < kids.size(); i++)
			kids[i]->set_animations(all_anim, matrices, animsize);
		}

};
int readtobone(string file,all_animations *all_animation, bone **proot,int flag);