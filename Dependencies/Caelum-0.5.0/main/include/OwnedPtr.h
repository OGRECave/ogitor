/*
This file is part of Caelum.
See http://www.ogre3d.org/wiki/index.php/Caelum 

Copyright (c) 2008 Caelum team. See Contributors.txt for details.

Caelum is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Caelum is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Caelum. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CAELUM__OWNED_PTR_H
#define CAELUM__OWNED_PTR_H

#include "CaelumPrerequisites.h"

namespace Caelum
{
    /** Default traits for Caelum::OwnedPtr.
     *
     *  This default traits class make OwnedPtr work like std::auto_ptr.
     *  Other Traits classes can derive from this and only customize some of
     *  the functions.
     *
     *  @see OwnedPtr
     */
    template<class PointedT>
    struct DefaultOwnedPtrTraits
    {
        /// The type of the inner member to hold in OwnedPtr
        typedef PointedT* InnerPointerType;

        /// Return an InnerPointerType repressenting a null value.
        static inline const InnerPointerType getNullValue() {
            return 0;
        }

        /// Convert InnerPointerType to a naked PointedT.
        static inline PointedT* getPointer (const InnerPointerType& inner) {
            return inner;
        }

        /// Destroy the inner value (and set null).
        static void destroy (InnerPointerType& inner)
        {
            delete inner;
            inner = 0;
        }
    };
    
    /** Template for smart pointers with strict unique ownership.
     *  A lot of objects in Ogre are created and destroyed through other
     *  "Manager" objects. Even though the memory for such objects is never
     *  actually leaked better lifetime control is frequently useful.
     *
     *  OwnedPtr is very similar in behaviour to std::auto_ptr but tries
     *  to mimic Ogre::SharedPtr method names. Only one OwnedPtr must exist to
     *  a certain object at any one time. Assignment and copy  construction will
     *  in fact pass away ownership and set the original OwnedPtr to null.
     *
     *  This very limited functionality makes OwnedPtr very efficient; it should
     *  have no overhead compared to doing the same thing manually.
     *
     *  OwnedPtr supports customization through a static traits class which
     *  can customize what happens when the OwnedPtr is destroyed. This makes
     *  it possible to use OwnedPtr classes for fine control over the lifetime
     *  of objects which are otherwise managed by an external class.
     *
     *  @see DefaultOwnedPtrTraits
     */
    template<class PointedT, typename TraitsT = DefaultOwnedPtrTraits<PointedT> >
    class OwnedPtr
    {
    private:
        /// Brings InnerPointerType as a type in this scope.
        typedef typename TraitsT::InnerPointerType InnerPointerType;

        /// Inner data member.
        InnerPointerType mInner;

	public:
        /** Change the inner value.
         *  This will destroy the old value and gain ownership of the new value.
         */
        void reset (const InnerPointerType& newInner = TraitsT::getNullValue()) {
            if (mInner == newInner) {
                return;
            }
            TraitsT::destroy (mInner);
            mInner = newInner;
        }

        /** Constructor; always initialize to 0.
         */
        OwnedPtr() { mInner = TraitsT::getNullValue (); }

        /** Non-virtual destructor (don't derive from this).
         */
        ~OwnedPtr() { setNull(); }

        /** Copy constructor; clears right-hand-side.
         */
        OwnedPtr(OwnedPtr& rhs)
        {
            if (&rhs != this) {
                this->reset (rhs.mInner);
                rhs.mInner = TraitsT::getNullValue ();
            }
        }
        
        /** Assignment
         */
        const OwnedPtr& operator=(OwnedPtr& rhs)
        {
            if (&rhs != this) {
                this->reset (rhs.mInner);
                rhs.mInner = TraitsT::getNullValue ();
            }
            return *this;
        }

        /// Check if this is null.
        bool isNull () const { return mInner != TraitsT::getNullValue (); }

        /// Set to null and destroy contents (if any).
        void setNull () { TraitsT::destroy (mInner); }

        PointedT* getPointer () const { return TraitsT::getPointer (mInner); }
        PointedT* get () const { return getPointer (); }
        PointedT* operator-> () const { return getPointer (); }
        PointedT& operator* () const{ return *getPointer (); }
    };

    /** Owned ptr traits for a movable object.
     *  This kind of pointer will remove the movable from the scene and destroy it.
     */
    template<class MovableT>
    struct MovableObjectOwnedPtrTraits: public DefaultOwnedPtrTraits<MovableT>
    {
        typedef MovableT* InnerPointerType;

        static void destroy (InnerPointerType& inner)
        {
            if (inner != 0) {
                //Ogre::LogManager::getSingletonPtr ()->logMessage (
                //        "OwnedPtr: Destroying movable object " + inner->getName ());
                inner->_getManager ()->destroyMovableObject (inner);
                inner = 0;
            }
        }
    };

    typedef OwnedPtr<Ogre::MovableObject, MovableObjectOwnedPtrTraits<Ogre::MovableObject> > MovableObjectPtr;
    typedef OwnedPtr<Ogre::BillboardChain, MovableObjectOwnedPtrTraits<Ogre::BillboardChain> > BillboardChainPtr;
    typedef OwnedPtr<Ogre::BillboardSet, MovableObjectOwnedPtrTraits<Ogre::BillboardSet> > BillboardSetPtr;
    typedef OwnedPtr<Ogre::Entity, MovableObjectOwnedPtrTraits<Ogre::Entity> > EntityPtr;
    typedef OwnedPtr<Ogre::Light, MovableObjectOwnedPtrTraits<Ogre::Light> > LightPtr;
    typedef OwnedPtr<Ogre::ManualObject, MovableObjectOwnedPtrTraits<Ogre::ManualObject> > ManualObjectPtr;
    typedef OwnedPtr<Ogre::ParticleSystem, MovableObjectOwnedPtrTraits<Ogre::ParticleSystem> > ParticleSystemPtr;

    /** Owned ptr traits for a scene node.
     *  Scene nodes are created and destroyed through the scene manager.
     *  @see SceneNodePtr
     */
    struct SceneNodeOwnedPtrTraits: public DefaultOwnedPtrTraits<Ogre::SceneNode>
    {
        static void destroy (InnerPointerType& inner)
        {
            if (inner) {
                //Ogre::LogManager::getSingletonPtr ()->logMessage (
                //        "OwnedPtr: Destroying scene node " + inner->getName ());
                inner->getCreator ()->destroySceneNode (inner->getName ());
                inner = 0;
            }
        }
    };

    typedef OwnedPtr<Ogre::SceneNode, SceneNodeOwnedPtrTraits> SceneNodePtr;

    /** OwnedPtr traits for uniquely-owned resources.
     *
     *  All ogre resources are tracked by a resource managed by name and can
     *  be globally referenced from multiple places. This traits class allows
     *  you to hold a pointer to a resource which you create and completely
     *  control.
     *
     *  The best example of this is a cloned material. It is frequently useful
     *  to create a clone of an existing material and tweak settings for one
     *  particular usage. After the clone is no longer useful the material must
     *  be explicitly removed from the MaterialManager. Otherwise an unloaded
     *  resource handle is leaked.
     *
     *  When the OwnedPtr gets out of scope the resource is removed from the
     *  manager. In debug mode this will also check that there are no other
     *  references to the destroyed resource.
     */
    template<class PointedT, class InnerT, class ManagerT>
    struct OwnedResourcePtrTraits
    {
        typedef InnerT InnerPointerType;

        static const InnerT getNullValue () {
            return InnerT();
        }

        static PointedT* getPointer (const InnerPointerType& inner) {
            return inner.getPointer ();
        }

        static void destroy (InnerPointerType& inner) {
            if (!inner.isNull ()) {
                //Ogre::LogManager::getSingletonPtr ()->logMessage (
                //        "OwnedPtr: Destroying owned resource " + inner->getName ());
                ManagerT::getSingletonPtr ()->remove (inner->getHandle ());
                assert (inner.unique () && "Resource pointer not unique after destruction");
            }
        }
    };

    typedef OwnedPtr <
        Ogre::Material,
        OwnedResourcePtrTraits <
            Ogre::Material,
            Ogre::MaterialPtr,
            Ogre::MaterialManager
        >
    > OwnedMaterialPtr;

    typedef OwnedPtr <
        Ogre::Mesh,
        OwnedResourcePtrTraits <
            Ogre::Mesh,
            Ogre::MeshPtr,
            Ogre::MeshManager
        >
    > OwnedMeshPtr;
}

#endif // CAELUM__OWNED_PTR_H
