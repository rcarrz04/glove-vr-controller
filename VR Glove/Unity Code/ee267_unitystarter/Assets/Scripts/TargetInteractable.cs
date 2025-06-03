using UnityEngine;

public class TargetInteractable : MonoBehaviour
{
    public void OnFist()
    {
        Debug.Log($"{gameObject.name} was grabbed!");
        Destroy(gameObject); // optional feature for future use: play animation/or increase score
    }
}
