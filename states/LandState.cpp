#include "../Game.h"
#include "State.h"
#include "StateIncludes.h"

LandState::LandState(Entity &parent)
  : State(parent, "land"), m_queue(Q_NONE), finished(false), can_q_roll(false) {
}

std::unique_ptr<State> LandState::handle_input() {
    if (!m_parent.m_physics->on_ground())
        return std::move(std::unique_ptr<State>(new FallState(m_parent)));

    if (finished && timer >= sf::seconds(.1)) {
        allow_change_dir();

        // IDLE
        if (m_queue == Q_NONE)
            return std::move(std::unique_ptr<State>(new IdleState(m_parent)));

        // JUMP
        else if (m_queue == Q_JUMP)
            return std::move(std::unique_ptr<State>(new JumpState(m_parent)));

        // ROLL
        else if (!m_parent.m_input->key_down("roll") && m_queue == Q_ROLL)
        {
            m_parent.m_animation->play_animation_dir("idle");
            return std::move(std::unique_ptr<State>(new RollState(m_parent)));
        }

        // Otherwise: IDLE
        return std::move(std::unique_ptr<State>(new IdleState(m_parent)));
    }

    // q_roll
    if (animation_id == "land") {
        if (!m_parent.m_input->key_down("roll"))
            can_q_roll = true;
        else if (can_q_roll)
            m_queue = Q_ROLL;

        // q_jump
        if (m_parent.m_input->key_pressed("jump"))
            m_queue = Q_JUMP;
    }

    return nullptr;
}

void LandState::update(float dt) {
    if (!m_parent.m_animation->current_animation()->is_active() && timer >= sf::seconds(.01) && animation_id == "land")
    {
        if (m_queue == Q_JUMP && m_parent.m_input->key_down("jump")
          && m_parent.m_input->key_down((m_parent.dir() == D_LEFT) ? "right":"left"))
            m_parent.m_animation->play_animation_dir(animation_id = "turn-to-prep-jump");
        else
            finished = true;
    }

    if (!m_parent.m_animation->current_animation()->is_active() && animation_id == "turn-to-prep-jump")
    {
        m_parent.set_dir((m_parent.dir() == D_LEFT) ? D_RIGHT : D_LEFT);
        finished = true;
    }

    timer += sf::seconds(dt);
}
