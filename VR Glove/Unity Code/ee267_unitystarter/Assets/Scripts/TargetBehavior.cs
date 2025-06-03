using UnityEngine;

public class TargetBehavior : MonoBehaviour
{
    public void DestroyTarget()
    {
        // Tell game object has been destroyed
        var spawnerToggle = FindObjectOfType<SpawnerToggle>();
        if (spawnerToggle != null)
        {
            spawnerToggle.AddScore();
        }

        Destroy(gameObject);
    }
}
