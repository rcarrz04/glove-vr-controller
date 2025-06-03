using UnityEngine;

public class GazeInteractable : MonoBehaviour
{
    public void OnGaze()
    {
        Debug.Log("Gaze triggered on: " + gameObject.name);
        GetComponent<Renderer>().material.color = Color.red;
    }
}
