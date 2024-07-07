using UnityEngine;
using UnityEditor;

namespace Project
{
    public class BasicFireZone : MonoBehaviour
    {
        public Material material;
        public Transform fireRingTransform;

        public float holeRadius { 
            get { return _holeRadius; }
            set
            { _holeRadius = Mathf.Clamp(value,0.01f,1f);
                fireRingTransform.localScale = _holeRadius * new Vector3(1, 1 / _holeRadius, 1);
                material.SetFloat("_HoleRadius", 0.9f * _holeRadius);
            } } 
        
        
        float _holeRadius = 1f;
    }


#if UNITY_EDITOR

[CustomEditor(typeof(BasicFireZone))]
[CanEditMultipleObjects]
public class BasicFireZoneEditor : Editor 
{
    float holeRadEdit ;
    
    public override void OnInspectorGUI() 
    {   
        BasicFireZone script = (BasicFireZone) target;
        DrawDefaultInspector();
        EditorGUILayout.BeginHorizontal();
        EditorGUILayout.LabelField("Hole Radius");
        holeRadEdit = EditorGUILayout.Slider(script.holeRadius, 0.01f, 1f);
        script.holeRadius = holeRadEdit; 
        EditorGUILayout.EndHorizontal();
    }
}

#endif
}

