/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file    GenericStereoFactor.h
 * @brief   A non-linear factor for stereo measurements
 * @author  Alireza Fathi
 * @author  Chris Beall
 */

#pragma once

#include <gtsam/slam/visualSLAM.h>
#include <gtsam/geometry/Cal3_S2.h>
#include <gtsam/geometry/StereoCamera.h>

namespace gtsam {

using namespace gtsam;

template<class VALUES=visualSLAM::Values, class KEY1=visualSLAM::PoseKey, class KEY2=visualSLAM::PointKey>
class GenericStereoFactor: public NonlinearFactor2<VALUES, KEY1, KEY2> {
private:

	// Keep a copy of measurement and calibration for I/O
	StereoPoint2 z_;
	boost::shared_ptr<Cal3_S2> K_;
	double baseline_;

public:

	// shorthand for base class type
	typedef NonlinearFactor2<VALUES, KEY1, KEY2> Base;

	// shorthand for a smart pointer to a factor
	typedef boost::shared_ptr<GenericStereoFactor> shared_ptr;

	typedef typename KEY1::Value CamPose;

	/**
	 * Default constructor
	 */
	GenericStereoFactor() : K_(new Cal3_S2(444, 555, 666, 777, 888)) {}

	/**
	 * Constructor
	 * @param z is the 2 dimensional location of point in image (the measurement)
	 * @param sigma is the standard deviation
	 * @param cameraFrameNumber is basically the frame number
	 * @param landmarkNumber is the index of the landmark
	 * @param K the constant calibration
	 */
	GenericStereoFactor(const StereoPoint2& z, const SharedGaussian& model, KEY1 j_pose,
			KEY2 j_landmark, const shared_ptrK& K, double baseline) :
		Base(model, j_pose, j_landmark), z_(z), K_(K), baseline_(baseline) {
	}

	~GenericStereoFactor() {}

	/**
	 * print
	 * @param s optional string naming the factor
	 */
	void print(const std::string& s) const {
		Base::print(s);
		z_.print(s + ".z");
	}

	/**
	 * equals
	 */
	bool equals(const GenericStereoFactor& f, double tol) const {
		const GenericStereoFactor* p = dynamic_cast<const GenericStereoFactor*> (&f);
		if (p == NULL)
			goto fail;
		//if (cameraFrameNumber_ != p->cameraFrameNumber_ || landmarkNumber_ != p->landmarkNumber_) goto fail;
		if (!z_.equals(p->z_, tol))
			goto fail;
		return true;

		fail: print("actual");
		p->print("expected");
		return false;
	}

	/** h(x)-z */
	Vector evaluateError(const CamPose& pose, const Point3& point,
			boost::optional<Matrix&> H1, boost::optional<Matrix&> H2) const {
		StereoCamera stereoCam(pose, *K_, baseline_);
		return (stereoCam.project(point, H1, H2) - z_).vector();
	}

	StereoPoint2 z() {
		return z_;
	}

private:
	/** Serialization function */
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_NVP(z_);
		ar & BOOST_SERIALIZATION_NVP(K_);
		ar & BOOST_SERIALIZATION_NVP(baseline_);
	}
};

// Typedef for general use
typedef GenericStereoFactor<visualSLAM::Values, visualSLAM::PoseKey, visualSLAM::PointKey> StereoFactor;

}
