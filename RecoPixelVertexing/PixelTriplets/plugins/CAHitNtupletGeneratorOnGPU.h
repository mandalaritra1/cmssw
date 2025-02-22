#ifndef RecoPixelVertexing_PixelTriplets_plugins_CAHitNtupletGeneratorOnGPU_h
#define RecoPixelVertexing_PixelTriplets_plugins_CAHitNtupletGeneratorOnGPU_h

#include <cuda_runtime.h>
// #include "CUDADataFormats/TrackingRecHit/interface/TrackingRecHit2DHeterogeneous.h"
// #include "CUDADataFormats/Track/interface/PixelTrackHeterogeneous.h"
#include "CUDADataFormats/Track/interface/TrackSoAHeterogeneousHost.h"
#include "CUDADataFormats/Track/interface/TrackSoAHeterogeneousDevice.h"

#include "CUDADataFormats/TrackingRecHit/interface/TrackingRecHitsUtilities.h"
#include "CUDADataFormats/TrackingRecHit/interface/TrackingRecHitSoAHost.h"
#include "CUDADataFormats/TrackingRecHit/interface/TrackingRecHitSoADevice.h"

#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "HeterogeneousCore/CUDAUtilities/interface/SimpleVector.h"

#include "CAHitNtupletGeneratorKernels.h"
#include "HelixFitOnGPU.h"

#include "GPUCACell.h"

namespace edm {
  class Event;
  class EventSetup;
  class ParameterSetDescription;
}  // namespace edm

template <typename TrackerTraits>
class CAHitNtupletGeneratorOnGPU {
public:
  using HitsView = TrackingRecHitSoAView<TrackerTraits>;
  using HitsConstView = TrackingRecHitSoAConstView<TrackerTraits>;
  using HitsOnGPU = TrackingRecHitSoADevice<TrackerTraits>;  //TODO move to OnDevice
  using HitsOnCPU = TrackingRecHitSoAHost<TrackerTraits>;    //TODO move to OnHost
  using hindex_type = typename TrackingRecHitSoA<TrackerTraits>::hindex_type;

  using HitToTuple = caStructures::HitToTupleT<TrackerTraits>;
  using TupleMultiplicity = caStructures::TupleMultiplicityT<TrackerTraits>;
  using OuterHitOfCell = caStructures::OuterHitOfCellT<TrackerTraits>;

  using GPUCACell = GPUCACellT<TrackerTraits>;
  using TrackSoAHost = TrackSoAHeterogeneousHost<TrackerTraits>;
  using TrackSoADevice = TrackSoAHeterogeneousDevice<TrackerTraits>;
  using HitContainer = typename TrackSoA<TrackerTraits>::HitContainer;
  using Tuple = HitContainer;

  using CellNeighborsVector = caStructures::CellNeighborsVectorT<TrackerTraits>;
  using CellTracksVector = caStructures::CellTracksVectorT<TrackerTraits>;

  using Quality = pixelTrack::Quality;

  using QualityCuts = pixelTrack::QualityCutsT<TrackerTraits>;
  using Params = caHitNtupletGenerator::ParamsT<TrackerTraits>;
  using Counters = caHitNtupletGenerator::Counters;

public:
  CAHitNtupletGeneratorOnGPU(const edm::ParameterSet& cfg, edm::ConsumesCollector&& iC)
      : CAHitNtupletGeneratorOnGPU(cfg, iC) {}
  CAHitNtupletGeneratorOnGPU(const edm::ParameterSet& cfg, edm::ConsumesCollector& iC);

  static void fillDescriptions(edm::ParameterSetDescription& desc);
  static void fillDescriptionsCommon(edm::ParameterSetDescription& desc);

  void beginJob();
  void endJob();

  TrackSoADevice makeTuplesAsync(HitsOnGPU const& hits_d, float bfield, cudaStream_t stream) const;

  TrackSoAHost makeTuples(HitsOnCPU const& hits_d, float bfield) const;

private:
  void buildDoublets(const HitsConstView& hh, cudaStream_t stream) const;

  void hitNtuplets(const HitsConstView& hh, const edm::EventSetup& es, bool useRiemannFit, cudaStream_t cudaStream);

  void launchKernels(const HitsConstView& hh, bool useRiemannFit, cudaStream_t cudaStream) const;

  Params m_params;

  Counters* m_counters = nullptr;
};

#endif  // RecoPixelVertexing_PixelTriplets_plugins_CAHitNtupletGeneratorOnGPU_h
