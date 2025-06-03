using UnityEngine;

public class GazeManager : MonoBehaviour
{
    public float maxDistance = 20f;
    public static GameObject currentTarget;

    void Update()
    {
        Ray ray = new Ray(transform.position, transform.forward);
        RaycastHit hit;

        if (Physics.Raycast(ray, out hit, maxDistance, ~LayerMask.GetMask("Ignore Raycast")))
        {
            GameObject hitObject = hit.collider.gameObject;


            if (hitObject != currentTarget)
            {
                // Unhover old
                if (currentTarget != null)
                {
                    var prev = currentTarget.GetComponent<GazeUIInteractable>();
                    if (prev != null)
                        prev.OnUnhover();
                }

                // Hover new
                currentTarget = hitObject;
                var now = currentTarget.GetComponent<GazeUIInteractable>();
                if (now != null)
                    now.OnHover();
            }
        }
        else
        {
            if (currentTarget != null)
            {
                var prev = currentTarget.GetComponent<GazeUIInteractable>();
                if (prev != null)
                    prev.OnUnhover();

                currentTarget = null;
            }
        }
    }
}
