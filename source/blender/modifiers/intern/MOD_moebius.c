/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2005 Blender Foundation.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): Daniel Dunbar
 *                 Ton Roosendaal,
 *                 Ben Batt,
 *                 Brecht Van Lommel,
 *                 Campbell Barton,
 * 				   Micah Fitch :)
 *
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file blender/modifiers/intern/MOD_none.c
 *  \ingroup modifiers
 */

#include <stdio.h>

#include "DNA_object_types.h"

#include "BLI_utildefines.h"
#include "BLI_math.h"

#include "BKE_library_query.h"
#include "BKE_modifier.h"
#include "BKE_cdderivedmesh.h"

#include "MOD_modifiertypes.h"
#include "MOD_util.h"

#include "DEG_depsgraph.h"
#include "DEG_depsgraph_query.h"


static void calc_moebius_transform(float hRot[4][4], float co[3], float p) {
	float norm;
	float h[4];
	float radius;
	int i;
	float eps;
	eps = 0.0000000000000001;

	radius = 1.0f;

	mul_v3_fl(co,1.0f/radius);
	norm = pow(co[0],p) + pow(co[1],p) + pow(co[2],p);

	mul_v3_v3fl(h, co, 2);

	h[3] = norm-1.0f;

	mul_v4_fl(h,1.0f/max_ff(1.0f+norm,eps));
	mul_m4_v4(hRot,h);
	copy_v3_v3(co,h);
	mul_v3_fl(co,radius/max_ff(1.0f-h[3],eps));
}

static void moebius_transform_verts(Object *control, Object *originObject, Object *target, DerivedMesh *dm,
                          float (*vertexCos)[3], int numVerts, bool relocalize, float norm_power)
{
	float imat[4];
	float leftQ[4];
	float rightQ[4];
	float hRot[4][4];
	float leftMat[4][4];
	float rightMat[4][4];

	float origin[3];
	float transformedTargetPosition[3];
	int a;

	invert_m4_m4(target->imat,target->obmat);

	mat4_to_quat(leftQ,control->obmat);
	mat4_to_quat(rightQ,control->obmat);

	leftMat[0][0] = leftMat[1][1] = leftMat[2][2] = leftMat[3][3] = leftQ[0];
	leftMat[0][1] = leftMat[2][3] = -leftQ[1];
	leftMat[0][2] = leftMat[3][1] = -leftQ[2];
	leftMat[0][3] = leftMat[1][2] = -leftQ[3];
	leftMat[2][1] = leftMat[3][0] = leftQ[3];
	leftMat[1][3] = leftMat[2][0] = leftQ[2];
	leftMat[1][0] = leftMat[3][2] = leftQ[1];

	rightMat[0][0] = rightMat[1][1] = rightMat[2][2] = rightMat[3][3] = rightQ[0];
	rightMat[0][1] = rightMat[3][2] = -rightQ[1];
	rightMat[0][2] = rightMat[1][3] = -rightQ[2];
	rightMat[0][3] = rightMat[2][1] = -rightQ[3];
	rightMat[1][2] = rightMat[3][0] = rightQ[3];
	rightMat[3][1] = rightMat[2][0] = rightQ[2];
	rightMat[1][0] = rightMat[2][3] = rightQ[1];

	mul_m4_m4m4(hRot,rightMat,leftMat);

	if(relocalize) {
		zero_v3(transformedTargetPosition);
		calc_moebius_transform(hRot, transformedTargetPosition, norm_power);
	}

	if(originObject != NULL) {
		copy_v3_v3(origin,originObject->obmat[3]);
	} else {
		copy_v3_v3(origin,control->obmat[3]);
	}
	for (a = 0; a < numVerts; a++) {
		mul_m4_v3(target->obmat,vertexCos[a]);
		sub_v3_v3(vertexCos[a],origin);

		calc_moebius_transform(hRot, vertexCos[a], norm_power);

		add_v3_v3(vertexCos[a],origin);
		mul_m4_v3(target->imat,vertexCos[a]);

		if(relocalize) {
			sub_v3_v3(vertexCos[a],transformedTargetPosition);
		}
	}
}


static CustomDataMask requiredDataMask(Object *UNUSED(ob), ModifierData *md)
{
	MoebiusModifierData *smd = (MoebiusModifierData *)md;
	CustomDataMask dataMask = 0;

	/* ask for vertexgroups if we need them
	if (smd->vgroup_name[0])
		dataMask |= CD_MASK_MDEFORMVERT;*/

	return dataMask;
}

static void deformVerts(ModifierData *md, Object *ob,
                        DerivedMesh *derivedData,
                        float (*vertexCos)[3],
                        int numVerts,
                        ModifierApplyFlag UNUSED(flag))
{
	MoebiusModifierData *mmd = (MoebiusModifierData *) md;
	DerivedMesh *dm = derivedData;
	CustomDataMask dataMask = requiredDataMask(ob, md);

	/* we implement requiredDataMask but thats not really useful since
	 * mesh_calc_modifiers pass a NULL derivedData */
	if (dataMask)
		dm = get_dm(ob, NULL, dm, NULL, 0,false);

	moebius_transform_verts(mmd->control,mmd->origin, ob, derivedData, vertexCos, numVerts, mmd->flags & eMoebiusModifierFlag_localize, mmd->norm_power);

	if (dm != derivedData)
		dm->release(dm);
}

static void deformVertsEM(
        ModifierData *md, Object *ob, struct BMEditMesh *em,
        DerivedMesh *derivedData, float (*vertexCos)[3], int numVerts)
{
	DerivedMesh *dm = derivedData;

	if (!derivedData) dm = CDDM_from_editbmesh(em, false, false);

	deformVerts(md, ob, dm, vertexCos, numVerts, 0);

	if (!derivedData) dm->release(dm);
}


/* Moebius Transform */
static void initData(ModifierData *md)
{
	MoebiusModifierData *smd = (MoebiusModifierData *) md;
	smd->norm_power = 2.0;
}

static void copyData(ModifierData *md, ModifierData *target)
{
	MoebiusModifierData *smd  = (MoebiusModifierData *)md;
	MoebiusModifierData *tsmd = (MoebiusModifierData *)target;

	tsmd->control  = smd->control;
	tsmd->origin  = smd->origin;
	tsmd->flags  = smd->flags;
	tsmd->norm_power = smd->norm_power;
}

static void foreachObjectLink(ModifierData *md, Object *ob,
                              ObjectWalkFunc walk, void *userData)
{
	MoebiusModifierData *mmd  = (MoebiusModifierData *)md;
	walk(userData, ob, &mmd->control, IDWALK_NOP);
	walk(userData, ob, &mmd->origin, IDWALK_NOP);
}

static int isDisabled(ModifierData *md, int UNUSED(userRenderParams))
{
	MoebiusModifierData *mmd = (MoebiusModifierData *) md;

	return !mmd->control;
}

static void updateDepgraph(ModifierData *md, DagForest *forest,
			   struct Main *UNUSED(bmain),
                           struct Scene *UNUSED(scene),
                           Object *UNUSED(ob),
                           DagNode *obNode)
{
	MoebiusModifierData *smd  = (MoebiusModifierData *)md;

	if (smd->control)
		dag_add_relation(forest, dag_get_node(forest, smd->control), obNode,  DAG_RL_DATA_DATA | DAG_RL_OB_DATA | DAG_RL_DATA_OB | DAG_RL_OB_OB, "Moebius Transformation Modifier");
	if (smd->origin)
		dag_add_relation(forest, dag_get_node(forest, smd->origin), obNode,  DAG_RL_DATA_DATA | DAG_RL_OB_DATA | DAG_RL_DATA_OB | DAG_RL_OB_OB, "Moebius Transformation Modifier");
}

ModifierTypeInfo modifierType_Moebius = {
	/* name */              "Moebius",
	/* structName */        "MoebiusModifierData",
	/* structSize */        sizeof(MoebiusModifierData),
	/* type */              eModifierTypeType_OnlyDeform,
	/* flags */             eModifierTypeFlag_AcceptsMesh |
	                        eModifierTypeFlag_AcceptsCVs |
							eModifierTypeFlag_SupportsEditmode,
	/* copyData */          copyData,
	/* deformVerts */       deformVerts,
	/* deformMatrices */    NULL,
	/* deformVertsEM */     deformVertsEM,
	/* deformMatricesEM */  NULL,
	/* applyModifier */     NULL,
	/* applyModifierEM */   NULL,
	/* initData */          initData,
	/* requiredDataMask */  requiredDataMask,
	/* freeData */          NULL,
	/* isDisabled */        isDisabled,
	/* updateDepgraph */    updateDepgraph,
	/* updateDepsgraph */	NULL,
	/* dependsOnTime */     NULL,
	/* dependsOnNormals */	NULL,
	/* foreachObjectLink */ foreachObjectLink,
	/* foreachIDLink */     NULL,
	/* foreachTexLink */    NULL,
};
