using UnityEngine;

public class FistInteractable : MonoBehaviour
{
    public void OnFistClosed()
    {
        Debug.Log("Fist closed on: " + gameObject.name);
        Destroy(gameObject); // Or play animation, score points, etc.
    }
}
